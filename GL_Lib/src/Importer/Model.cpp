#include "Model.h"

#include <iostream>

#include "Camera.h"
#include "Frustum.h"
#include "Renderer.h"
#include <unordered_map>

#include "BSP/BSPNode.h"
namespace {
    // Returns true iff ALL 8 corners of this AABB are on the opposite side of the plane than the camera.
    inline bool aabbCompletelyOnOppositeSide(
        const glm::vec3& wMin, const glm::vec3& wMax,
        const gllib::BSPPlane* plane, bool cameraInFront)
    {
        if (!plane) return false;

        const glm::vec3 corners[8] = {
            {wMin.x, wMin.y, wMin.z}, {wMax.x, wMin.y, wMin.z},
            {wMin.x, wMax.y, wMin.z}, {wMax.x, wMax.y, wMin.z},
            {wMin.x, wMin.y, wMax.z}, {wMax.x, wMin.y, wMax.z},
            {wMin.x, wMax.y, wMax.z}, {wMax.x, wMax.y, wMax.z}
        };

        for (int i = 0; i < 8; ++i)
        {
            if (plane->isPointInFront(corners[i]) == cameraInFront)
                return false;
        }
        return true;
    }

    // True iff SOME node (this transform's own AABB or any descendant) lies on the camera side or straddles.
    bool subtreeHasAnyOnCameraSide(const gllib::Transform* t, const gllib::BSPPlane* plane, bool cameraInFront)
    {
        if (!plane) return true;

        const glm::vec3 ownMin = t->getWorldAABBMin();
        const glm::vec3 ownMax = t->getWorldAABBMax();

        if (!aabbCompletelyOnOppositeSide(ownMin, ownMax, plane, cameraInFront))
            return true;

        for (const gllib::Transform* c : t->children)
        {
            if (subtreeHasAnyOnCameraSide(c, plane, cameraInFront))
                return true;
        }
        return false;
    }
}
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


    void Model::drawHierarchicalWithBSP(const Frustum& frustum,
                                    const BSPPlane* bspPlane,
                                    bool cameraInFront)
    {
        // If there is no plane, nothing to do here (but keep signature for compatibility)
        if (!bspPlane)
        {
            drawHierarchical(frustum);
            return;
        }

        // Strict hierarchical BSP reject for this subtree
        if (!subtreeHasAnyOnCameraSide(&transform, bspPlane, cameraInFront))
            return;

        // No parent-level frustum cut here; rely on per-mesh frustum tests to avoid
        // dropping visible children due to an off-screen parent.

        // Draw meshes bound to the root transform
        for (Mesh& mesh : meshes)
        {
            if (mesh.associatedTransform != &transform) continue;

            const glm::mat4 worldM = transform.getTransformMatrix();
            const glm::vec3 mn = mesh.minAABB, mx = mesh.maxAABB;

            const glm::vec3 corners[8] = {
                {mn.x, mn.y, mn.z}, {mx.x, mn.y, mn.z},
                {mn.x, mx.y, mn.z}, {mx.x, mx.y, mn.z},
                {mn.x, mn.y, mx.z}, {mx.x, mn.y, mx.z},
                {mn.x, mx.y, mx.z}, {mx.x, mx.y, mx.z}
            };

            glm::vec3 wMin( FLT_MAX), wMax(-FLT_MAX);
            for (int i = 0; i < 8; ++i)
            {
                const glm::vec3 wc = glm::vec3(worldM * glm::vec4(corners[i], 1.0f));
                wMin = glm::min(wMin, wc);
                wMax = glm::max(wMax, wc);
            }

            if (!frustum.isAABBInside(wMin, wMax))
                continue;

            if (aabbCompletelyOnOppositeSide(wMin, wMax, bspPlane, cameraInFront))
                continue;

            Renderer::drawModel3D(mesh.VAO, mesh.indices.size(), worldM, mesh.textures);
        }

        // Descend to children (skip whole branch if fully opposite)
        for (Transform* child : transform.children)
        {
            if (!subtreeHasAnyOnCameraSide(child, bspPlane, cameraInFront))
                continue;

            drawChildTransformWithBSP(child, frustum, bspPlane, cameraInFront);
        }
    }


    void Model::drawChildTransformWithBSP(Transform* childTransform,
                                      const Frustum& frustum,
                                      const BSPPlane* bspPlane,
                                      bool cameraInFront)
    {
        if (!childTransform) return;

        // Strict hierarchical BSP reject for this child subtree
        if (!subtreeHasAnyOnCameraSide(childTransform, bspPlane, cameraInFront))
            return;

        // Draw meshes attached to this child
        for (Mesh& mesh : meshes)
        {
            if (mesh.associatedTransform != childTransform) continue;

            const glm::mat4 worldM = childTransform->getTransformMatrix();
            const glm::vec3 mn = mesh.minAABB, mx = mesh.maxAABB;

            const glm::vec3 corners[8] = {
                {mn.x, mn.y, mn.z}, {mx.x, mn.y, mn.z},
                {mn.x, mx.y, mn.z}, {mx.x, mx.y, mn.z},
                {mn.x, mn.y, mx.z}, {mx.x, mn.y, mx.z},
                {mn.x, mx.y, mx.z}, {mx.x, mx.y, mx.z}
            };

            glm::vec3 wMin( FLT_MAX), wMax(-FLT_MAX);
            for (int i = 0; i < 8; ++i)
            {
                const glm::vec3 wc = glm::vec3(worldM * glm::vec4(corners[i], 1.0f));
                wMin = glm::min(wMin, wc);
                wMax = glm::max(wMax, wc);
            }

            if (!frustum.isAABBInside(wMin, wMax))
                continue;

            if (aabbCompletelyOnOppositeSide(wMin, wMax, bspPlane, cameraInFront))
                continue;

            Renderer::drawModel3D(mesh.VAO, mesh.indices.size(), worldM, mesh.textures);
        }

        // Recurse to grandchildren
        for (Transform* gc : childTransform->children)
        {
            if (!subtreeHasAnyOnCameraSide(gc, bspPlane, cameraInFront))
                continue;

            drawChildTransformWithBSP(gc, frustum, bspPlane, cameraInFront);
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
            min.x, min.y, min.z, max.x, min.y, min.z,
            max.x, min.y, min.z, max.x, min.y, max.z,
            max.x, min.y, max.z, min.x, min.y, max.z,
            min.x, min.y, max.z, min.x, min.y, min.z,

            // Top face edges
            min.x, max.y, min.z, max.x, max.y, min.z,
            max.x, max.y, min.z, max.x, max.y, max.z,
            max.x, max.y, max.z, min.x, max.y, max.z,
            min.x, max.y, max.z, min.x, max.y, min.z,

            // Vertical edges
            min.x, min.y, min.z, min.x, max.y, min.z,
            max.x, min.y, min.z, max.x, max.y, min.z,
            max.x, min.y, max.z, max.x, max.y, max.z,
            min.x, min.y, max.z, min.x, max.y, max.z
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

    void Model::drawAllAABBsDebug(const glm::mat4& view, const glm::mat4& projection)
    {
        if (!aabbInitialized)
            initializeAABBVisualization();

        // Draw root transform AABB
        drawTransformAABB(&transform, view, projection);

        // Recursively draw all children AABBs
        std::function<void(Transform*)> drawChildrenAABBs = [&](Transform* t)
        {
            for (Transform* child : t->children)
            {
                drawTransformAABB(child, view, projection);
                drawChildrenAABBs(child);
            }
        };

        drawChildrenAABBs(&transform);
    }

    void Model::drawTransformAABB(Transform* t, const glm::mat4& view, const glm::mat4& projection)
    {
        glm::vec3 min = t->getWorldAABBMin();
        glm::vec3 max = t->getWorldAABBMax();

        // Skip if invalid AABB
        if (min.x > max.x || min.y > max.y || min.z > max.z)
            return;

        std::vector<float> vertices = {
            // Bottom face edges
            min.x, min.y, min.z, max.x, min.y, min.z,
            max.x, min.y, min.z, max.x, min.y, max.z,
            max.x, min.y, max.z, min.x, min.y, max.z,
            min.x, min.y, max.z, min.x, min.y, min.z,

            // Top face edges
            min.x, max.y, min.z, max.x, max.y, min.z,
            max.x, max.y, min.z, max.x, max.y, max.z,
            max.x, max.y, max.z, min.x, max.y, max.z,
            min.x, max.y, max.z, min.x, max.y, min.z,

            // Vertical edges
            min.x, min.y, min.z, min.x, max.y, min.z,
            max.x, min.y, min.z, max.x, max.y, min.z,
            max.x, min.y, max.z, max.x, max.y, max.z,
            min.x, min.y, max.z, min.x, max.y, max.z
        };

        glBindVertexArray(aabbVAO);
        glBindBuffer(GL_ARRAY_BUFFER, aabbVBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

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
            // Use cyan color for individual mesh AABBs
            glm::vec4 color(0.0f, 1.0f, 1.0f, 1.0f);
            glUniform4fv(colorLoc, 1, glm::value_ptr(color));
        }

        glDrawArrays(GL_LINES, 0, 24);
        glBindVertexArray(0);
    }
}
