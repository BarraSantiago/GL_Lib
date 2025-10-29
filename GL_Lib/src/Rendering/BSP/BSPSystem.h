#pragma once
#include "BSPNode.h"
#include <vector>
#include <memory>

namespace gllib
{
    class DLLExport BSPSystem
    {
    private:
        std::unique_ptr<BSPNode> rootNode;
        std::vector<Model*> allModels;
        std::unique_ptr<BSPPlaneVisualizer> planeVisualizer;

    public:
        BSPSystem();
        ~BSPSystem() = default;

        void addModel(Model* model);
        void removeModel(Model* model);
        void buildBSP(const std::vector<BSPPlane>& separatingPlanes);
        void render(const Camera& camera);
        void renderDebug(const Camera& camera, bool drawAABB = true);
        void clear();

    private:
        void buildBSPRecursive(BSPNode* node, const std::vector<BSPPlane>& planes, int planeIndex);
    };
}
