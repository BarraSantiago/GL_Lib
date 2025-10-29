#include "Model.h"

#include <iostream>

#include "Camera.h"
#include "Frustum.h"
#include "Renderer.h"
#include <unordered_map>

#include "BSP/BSPNode.h"

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

        for (const Mesh& mesh : meshes)
        {
            // Check if mesh has valid vertices (no zero AABB)
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
        
        if (aabbInitialized)
        {
            glDeleteBuffers(1, &aabbVBO);
            glDeleteVertexArrays(1, &aabbVAO);
        }
        
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
        // Set up frustum culling
        Frustum frustum;
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix();
        glm::mat4 viewProjection = projection * view;
        frustum.extractFromMatrix(viewProjection);
        
        drawWithFrustum(frustum);
    }
    
    void Model::drawWithFrustum(const Frustum& frustum)
    {
        // Update transforms and calculate AABBs
        transform.updateTRSAndAABB();
        
        // Detailed frustum culling and hierarchical rendering
        drawHierarchical(frustum);
    }
    
    void Model::drawHierarchical(const Frustum& frustum)
    {
        // Check if this transform's AABB is in frustum
        glm::vec3 aabbMin = transform.getWorldAABBMin();
        glm::vec3 aabbMax = transform.getWorldAABBMax();
        if (!frustum.isAABBInside(aabbMin, aabbMax))
        {
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
    
    void Model::drawWithFrustumAndBSP(const Frustum& frustum, const BSPPlane* bspPlane, const glm::vec3& cameraPos)
    {
        transform.updateTRSAndAABB();
        
        if (bspPlane)
        {
            // Check if entire model AABB is on opposite side of plane from camera
            glm::vec3 modelMin = transform.getWorldAABBMin();
            glm::vec3 modelMax = transform.getWorldAABBMax();
            glm::vec3 modelCenter = (modelMin + modelMax) * 0.5f;
            
            bool cameraInFront = bspPlane->isPointInFront(cameraPos);
            bool modelInFront = bspPlane->isPointInFront(modelCenter);
            
            // If model is entirely on opposite side, don't render at all
            if (cameraInFront != modelInFront)
            {
                // Check all 8 corners of AABB to confirm entire model is on wrong side
                glm::vec3 corners[8] = {
                    glm::vec3(modelMin.x, modelMin.y, modelMin.z),
                    glm::vec3(modelMax.x, modelMin.y, modelMin.z),
                    glm::vec3(modelMin.x, modelMax.y, modelMin.z),
                    glm::vec3(modelMax.x, modelMax.y, modelMin.z),
                    glm::vec3(modelMin.x, modelMin.y, modelMax.z),
                    glm::vec3(modelMax.x, modelMin.y, modelMax.z),
                    glm::vec3(modelMin.x, modelMax.y, modelMax.z),
                    glm::vec3(modelMax.x, modelMax.y, modelMax.z)
                };
                
                bool allOnWrongSide = true;
                for (int i = 0; i < 8; i++)
                {
                    if (bspPlane->isPointInFront(corners[i]) == cameraInFront)
                    {
                        allOnWrongSide = false;
                        break;
                    }
                }
                
                if (allOnWrongSide)
                    return;
            }
            
            drawHierarchicalWithBSP(frustum, bspPlane, cameraInFront);
        }
        else
        {
            drawHierarchical(frustum);
        }
    }
    
    void Model::drawHierarchicalWithBSP(const Frustum& frustum, const BSPPlane* bspPlane, bool cameraInFront)
    {
        glm::vec3 aabbMin = transform.getWorldAABBMin();
        glm::vec3 aabbMax = transform.getWorldAABBMax();
        if (!frustum.isAABBInside(aabbMin, aabbMax))
        {
            return;
        }
    
        // Check if mesh is on same side as camera
        glm::vec3 meshCenter = (aabbMin + aabbMax) * 0.5f;
        bool meshInFront = bspPlane->isPointInFront(meshCenter);
        
        if (meshInFront == cameraInFront)
        {
            for (Mesh& mesh : meshes)
            {
                if (mesh.associatedTransform == &transform)
                {
                    Renderer::drawModel3D(mesh.VAO, mesh.indices.size(),
                                          transform.getTransformMatrix(), mesh.textures);
                }
            }
        }
    
        for (Transform* child : transform.children)
        {
            drawChildTransformWithBSP(child, frustum, bspPlane, cameraInFront);
        }
    }
    
    void Model::drawChildTransformWithBSP(Transform* childTransform, const Frustum& frustum, 
                                          const BSPPlane* bspPlane, bool cameraInFront)
    {
        glm::vec3 aabbMin = childTransform->getWorldAABBMin();
        glm::vec3 aabbMax = childTransform->getWorldAABBMax();
        if (!frustum.isAABBInside(aabbMin, aabbMax))
        {
            return;
        }
    
        // Check if child mesh is on same side as camera
        glm::vec3 meshCenter = (aabbMin + aabbMax) * 0.5f;
        bool meshInFront = bspPlane->isPointInFront(meshCenter);
        
        if (meshInFront == cameraInFront)
        {
            for (Mesh& mesh : meshes)
            {
                if (mesh.associatedTransform == childTransform)
                {
                    Renderer::drawModel3D(mesh.VAO, mesh.indices.size(),
                                          childTransform->getTransformMatrix(), mesh.textures);
                }
            }
        }
    
        for (Transform* grandchild : childTransform->children)
        {
            drawChildTransformWithBSP(grandchild, frustum, bspPlane, cameraInFront);
        }
    }

    void Model::initializeAABBVisualization()
    {
        if (aabbInitialized)
            return;
        
        glGenVertexArrays(1, &aabbVAO);
        glGenBuffers(1, &aabbVBO);
        aabbInitialized = true;
    }
    
    void Model::drawAABBDebug(const glm::mat4& view, const glm::mat4& projection)
    {
        if (!aabbInitialized)
            initializeAABBVisualization();
        
        glm::vec3 min = transform.getWorldAABBMin();
        glm::vec3 max = transform.getWorldAABBMax();
        
        // Define 12 edges of AABB as line segments
        std::vector<float> vertices = {
            // Bottom face edges
            min.x, min.y, min.z,  max.x, min.y, min.z,
            max.x, min.y, min.z,  max.x, min.y, max.z,
            max.x, min.y, max.z,  min.x, min.y, max.z,
            min.x, min.y, max.z,  min.x, min.y, min.z,
            
            // Top face edges
            min.x, max.y, min.z,  max.x, max.y, min.z,
            max.x, max.y, min.z,  max.x, max.y, max.z,
            max.x, max.y, max.z,  min.x, max.y, max.z,
            min.x, max.y, max.z,  min.x, max.y, min.z,
            
            // Vertical edges
            min.x, min.y, min.z,  min.x, max.y, min.z,
            max.x, min.y, min.z,  max.x, max.y, min.z,
            max.x, min.y, max.z,  max.x, max.y, max.z,
            min.x, min.y, max.z,  min.x, max.y, max.z
        };
        
        glBindVertexArray(aabbVAO);
        glBindBuffer(GL_ARRAY_BUFFER, aabbVBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
        
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        // Draw with current shader (should be solid color shader)
        GLint currentProgram;
        glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
        
        glm::mat4 mvp = projection * view;
        GLint mvpLoc = glGetUniformLocation(currentProgram, "u_MVP");
        if (mvpLoc != -1)
        {
            glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));
        }
        
        GLint colorLoc = glGetUniformLocation(currentProgram, "u_Color");
        if (colorLoc != -1)
        {
            glm::vec4 color(0.0f, 1.0f, 0.0f, 1.0f); // Green color
            glUniform4fv(colorLoc, 1, glm::value_ptr(color));
        }
        
        glDrawArrays(GL_LINES, 0, 24);
        glBindVertexArray(0);
    }
}
