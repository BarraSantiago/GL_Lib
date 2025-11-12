#pragma once
#include <vector>
#include <memory>
#include "BSPNode.h"

namespace gllib
{
    class Model;
    class Camera;

    class DLLExport BSPSystem
    {
    private:
        std::vector<Model*> models_;
        std::unique_ptr<BSPNode> root_;
        std::vector<BSPPlane> planes_;
        BSPPlane activePlane_;
        bool hasActivePlane_;

        bool aabbFullyOpposite(const glm::vec3& wMin, const glm::vec3& wMax, const BSPPlane& plane, bool cameraInFront);

    public:
        BSPSystem();

        void addModel(Model* model);
        void removeModel(Model* model);
        void addPlane(const BSPPlane& plane);
        void clearPlanes();
        const std::vector<BSPPlane>& getPlanes() const { return planes_; }

        void buildBSP(const std::vector<BSPPlane>& planes);
        void buildBSP(); // Build with current planes
        void render(const Camera& camera);
        void renderDebug(const Camera& camera, bool drawAABB);
        void clear();
    };
}
