#include "Model.h"

#include <iostream>

#include "Camera.h"
#include "Frustum.h"
#include "Renderer.h"
#include <unordered_map>

namespace gllib
{
    std::unordered_map<Transform*, Model*> Model::transformToModelMap;

    Model::Model(std::string const& path, bool gamma)
    {
        // Initialize transform first
        transform.position = glm::vec3(0.0f);
        transform.scale = glm::vec3(1.0f);
        transform.rotationQuat = {1.0f, 0.0f, 0.0f, 0.0f};

        ModelLoader::loadModel(path, meshes, gamma, &transform);

        // Initialize with invalid AABB first
        transform.localAABBMin = glm::vec3(FLT_MAX);
        transform.localAABBMax = glm::vec3(-FLT_MAX);

        bool hasValidGeometry = false;

        for (const auto& mesh : meshes)
        {
            // Check if mesh has valid vertices (non-zero AABB)
            if (mesh.minAABB != mesh.maxAABB)
            {
                if (!hasValidGeometry)
                {
                    transform.localAABBMin = mesh.minAABB;
                    transform.localAABBMax = mesh.maxAABB;
                    hasValidGeometry = true;
                }
                else
                {
                    transform.localAABBMin = glm::min(transform.localAABBMin, mesh.minAABB);
                    transform.localAABBMax = glm::max(transform.localAABBMax, mesh.maxAABB);
                }
            }
        }

        // If no valid geometry, set a small default AABB
        if (!hasValidGeometry)
        {
            transform.localAABBMin = glm::vec3(-0.5f);
            transform.localAABBMax = glm::vec3(0.5f);
        }

        registerModel(&transform, this);

        std::cout << "Model loaded with " << transform.children.size() << " child transforms" << std::endl;
    }

    Model::~Model()
    {
        unregisterModel(&transform);

        // Clean up dynamically created child transforms
        std::function<void(Transform*)> cleanupChildren = [&](Transform* t)
        {
            for (Transform* child : t->children)
            {
                cleanupChildren(child);
                delete child;
            }
            t->children.clear();
        };

        cleanupChildren(&transform);
    }

    void Model::draw(const Camera& camera)
    {
        // Single recursive function to update transforms and calculate AABBs
        transform.updateTRSAndAABB();
    
        // Set up frustum culling
        Frustum frustum;
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix();
        glm::mat4 viewProjection = projection * view;
        frustum.extractFromMatrix(viewProjection);
    
        // Render with hierarchical culling
        drawHierarchical(frustum);
    }

    void Model::drawHierarchical(const Frustum& frustum)
    {
        // Check if this transform's AABB is in frustum
        glm::vec3 aabbMin = transform.getWorldAABBMin();
        glm::vec3 aabbMax = transform.getWorldAABBMax();
        if (!frustum.isAABBInside(aabbMin, aabbMax))
        {
            std::cout << "Transform AABB not in frustum, skipping draw." << std::endl;
            return;
        }
    
        // Draw meshes associated with this transform
        for (Mesh& mesh : meshes)
        {
            if (mesh.associatedTransform == &transform)
            {
                Renderer::drawModel3D(mesh.VAO, mesh.indices.size(),
                                      transform.getTransformMatrix(), mesh.textures);
            }
        }
    
        // Recursively draw children
        for (Transform* child : transform.children)
        {
            drawChildTransform(child, frustum);
        }
    }
    
    void Model::drawChildTransform(Transform* childTransform, const Frustum& frustum)
    {
        // Check if this child transform's AABB is in frustum
        glm::vec3 aabbMin = childTransform->getWorldAABBMin();
        glm::vec3 aabbMax = childTransform->getWorldAABBMax();
        if (!frustum.isAABBInside(aabbMin, aabbMax))
        {
            return;
        }
    
        // Draw meshes associated with this child transform
        for (Mesh& mesh : meshes) 
        {
            if (mesh.associatedTransform == childTransform)
            {
                Renderer::drawModel3D(mesh.VAO, mesh.indices.size(),
                                      childTransform->getTransformMatrix(), mesh.textures);
            }
        }
    
        // Recursively draw grandchildren
        for (Transform* grandchild : childTransform->children)
        {
            drawChildTransform(grandchild, frustum);
        }
    }

    void Model::draw()
    {
    }

    void Model::registerModel(Transform* transform, Model* model)
    {
        transformToModelMap[transform] = model;
    }

    void Model::unregisterModel(Transform* transform)
    {
        transformToModelMap.erase(transform);
    }

    Model* Model::getModelFromTransform(Transform* transform)
    {
        std::unordered_map<Transform*, Model*>::iterator it = transformToModelMap.find(transform);
        return (it != transformToModelMap.end()) ? it->second : nullptr;
    }
}
