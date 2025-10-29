#pragma once
#include <memory>
#include <vector>
#include "BSPNode.h"

namespace gllib {

    class Camera;
    class Model;

    class DLLExport BSPSystem {
    public:
        BSPSystem();
        ~BSPSystem() = default;

        void addModel(Model* model);
        void removeModel(Model* model);

        // Builds or updates the active plane set. We use planes[0] as the active culling plane.
        void buildBSP(const std::vector<BSPPlane>& planes);

        // Strict culling: if a model’s hierarchical AABB is fully on the opposite side of the active plane,
        // it will NOT be rendered.
        void render(const Camera& camera);

        // Optional debug helpers
        void renderDebug(const Camera& camera, bool drawAABB = true);
        void clear();

    private:
        // Utility
        static bool aabbFullyOpposite(const glm::vec3& wMin,
                                      const glm::vec3& wMax,
                                      const BSPPlane&  plane,
                                      bool             cameraInFront);

    private:
        // Kept for future extensions / tooling (not used for the strict culling decision)
        std::unique_ptr<BSPNode> root_;

        // Registered content
        std::vector<Model*> models_;

        // Active culling plane (single plane based on your requirement)
        bool     hasActivePlane_ = false;
        BSPPlane activePlane_{};
    };

} // namespace gllib
