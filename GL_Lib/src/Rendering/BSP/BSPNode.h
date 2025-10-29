#pragma once
#include "deps.h"
#include "glm.hpp"
#include <vec3.hpp>
#include <vector>
#include <memory>

#include "Rendering/Frustum.h"
#include "Importer/Model.h"

namespace gllib
{

    struct DLLExport BSPPlane
    {
        glm::vec3 normal;
        float distance;
        
        float distanceToPoint(const glm::vec3& point) const
        {
            return glm::dot(normal, point) + distance;
        }
        
        bool isPointInFront(const glm::vec3& point) const
        {
            return distanceToPoint(point) >= 0.0f;
        }
    };

    class DLLExport BSPPlaneVisualizer
    {
    private:
        unsigned int VAO, VBO;
        std::vector<float> vertices;
        
    public:
        BSPPlaneVisualizer(const BSPPlane& plane, float size = 50.0f);
        ~BSPPlaneVisualizer();
        void draw(const glm::mat4& view, const glm::mat4& projection);
    };

    class DLLExport BSPNode
    {
    public:
        BSPPlane plane;
        std::unique_ptr<BSPNode> frontChild;
        std::unique_ptr<BSPNode> backChild;
        std::vector<Model*> models;
        
        BSPNode() = default;
        BSPNode(const BSPPlane& splitPlane) : plane(splitPlane) {}

        void collectVisibleModels(const glm::vec3& cameraPos, const Frustum& frustum,std::vector<Model*>& visibleModels);
        bool isLeaf() const { return !frontChild && !backChild; }
        
        void addModel(Model* model);
        void render(const Camera& camera, const Frustum& frustum);
        
    private:
        void renderNode(const glm::vec3& cameraPos, const Frustum& frustum);
    };
}
