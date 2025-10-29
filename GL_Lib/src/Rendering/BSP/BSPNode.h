#pragma once
#include <memory>
#include <vector>
#include "glm/glm.hpp"
#include "deps.h"
#include "Rendering/Frustum.h"

namespace gllib {

    struct DLLExport BSPPlane {
        glm::vec3 normal {0,0,1};
        float     distance = 0.0f; // plane: n·x + d = 0

        inline float distanceToPoint(const glm::vec3& p) const {
            return glm::dot(normal, p) + distance;
        }
        inline bool isPointInFront(const glm::vec3& p) const {
            return distanceToPoint(p) >= 0.0f;
        }
    };

    // Forward decls
    class Model;
    class Camera;

    class DLLExport BSPNode {
    public:
        BSPPlane plane{};
        std::unique_ptr<BSPNode> frontChild;
        std::unique_ptr<BSPNode> backChild;

        // (Kept only for completeness; rendering no longer depends on node membership.)
        std::vector<Model*> models;

        BSPNode() = default;
        explicit BSPNode(const BSPPlane& p) : plane(p) {}

        bool isLeaf() const { return !frontChild && !backChild; }

        // No-op placement (kept for API compatibility)
        void addModel(Model* model);

        // Not used for visibility any more; kept for optional debug/ traversal
        void collectVisibleModels(const glm::vec3& cameraPos,
                                  const Frustum&   frustum,
                                  std::vector<Model*>& out);

        // Optional lightweight debug hook (safe to remove if unused)
        void render(const Camera& camera, const Frustum& frustum);
    };

} // namespace gllib
