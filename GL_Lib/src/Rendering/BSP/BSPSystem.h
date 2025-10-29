#pragma once
#include <memory>
#include <vector>
#include "BSPNode.h"

namespace gllib
{
    class Camera;
    class Model;

    class DLLExport BSPSystem
    {
    public:
        BSPSystem();
        ~BSPSystem() = default;

        void addModel(Model* model);
        void removeModel(Model* model);
        void buildBSP(const std::vector<BSPPlane>& planes);
        void render(const Camera& camera);
        void renderDebug(const Camera& camera, bool drawAABB = true);
        void clear();

    private:
        static bool aabbFullyOpposite(const glm::vec3& wMin, const glm::vec3& wMax, const BSPPlane& plane, bool cameraInFront);
        std::unique_ptr<BSPNode> root_;
        std::vector<Model*> models_;
        bool hasActivePlane_ = false;
        BSPPlane activePlane_{};
    };
}
