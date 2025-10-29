#pragma once
#include <memory>
#include <vector>
#include "glm/glm.hpp"
#include "deps.h"
#include "Rendering/Frustum.h"

namespace gllib {

    struct DLLExport BSPPlane {
        glm::vec3 normal {0,0,1};
        float distance = 0.0f;

        float distanceToPoint(const glm::vec3& p) const {
            return glm::dot(normal, p) + distance;
        }

        bool isPointInFront(const glm::vec3& p) const {
            return distanceToPoint(p) >= 0.0f;
        }
    };

    class Model;
    class Camera;

    class DLLExport BSPNode {
    public:
        BSPPlane plane{};
        std::unique_ptr<BSPNode> frontChild;
        std::unique_ptr<BSPNode> backChild;

        std::vector<Model*> models;

        BSPNode() = default;
        explicit BSPNode(const BSPPlane& p) : plane(p) {}

        bool isLeaf() const { return !frontChild && !backChild; }

        void addModel(Model* model);

        void collectVisibleModels(const glm::vec3& cameraPos,const Frustum&   frustum,std::vector<Model*>& out);
    };

}
