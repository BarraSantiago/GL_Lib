#include "BSPSystem.h"
#include "Camera.h"
#include "Frustum.h"
#include <algorithm>

namespace gllib
{
    BSPSystem::BSPSystem()
    {
        rootNode = std::make_unique<BSPNode>();
    }

    void BSPSystem::addModel(Model* model)
    {
        if (model && std::find(allModels.begin(), allModels.end(), model) == allModels.end())
        {
            allModels.push_back(model);
            if (rootNode)
                rootNode->addModel(model);
        }
    }

    void BSPSystem::removeModel(Model* model)
    {
        std::vector<Model*>::iterator it = std::find(allModels.begin(), allModels.end(), model);
        if (it != allModels.end())
        {
            allModels.erase(it);
        }
    }

    void BSPSystem::buildBSP(const std::vector<BSPPlane>& separatingPlanes)
    {
        if (separatingPlanes.empty())
        {
            rootNode = std::make_unique<BSPNode>();
            planeVisualizer.reset();
            for (Model* model : allModels)
            {
                rootNode->addModel(model);
            }
            return;
        }

        rootNode = std::make_unique<BSPNode>(separatingPlanes[0]);
        buildBSPRecursive(rootNode.get(), separatingPlanes, 0);

        // Create visualizer for the first plane
        planeVisualizer = std::make_unique<BSPPlaneVisualizer>(separatingPlanes[0], 50.0f);

        for (Model* model : allModels)
        {
            rootNode->addModel(model);
        }
    }

    void BSPSystem::renderDebug(const Camera& camera, bool drawAABB)
    {
        if (planeVisualizer)
        {
            glDisable(GL_DEPTH_TEST);
            planeVisualizer->draw(camera.getViewMatrix(), camera.getProjectionMatrix());
            glEnable(GL_DEPTH_TEST);
        }
        
        if (drawAABB)
        {
            for (Model* model : allModels)
            {
                model->drawAABBDebug(camera.getViewMatrix(), camera.getProjectionMatrix());
            }
        }
    }

    void BSPSystem::buildBSPRecursive(BSPNode* node, const std::vector<BSPPlane>& planes, int planeIndex)
    {
        if (planeIndex >= planes.size() - 1)
            return;

        int nextPlaneIndex = planeIndex + 1;
        node->frontChild = std::make_unique<BSPNode>(planes[nextPlaneIndex]);
        node->backChild = std::make_unique<BSPNode>(planes[nextPlaneIndex]);

        buildBSPRecursive(node->frontChild.get(), planes, nextPlaneIndex);
        buildBSPRecursive(node->backChild.get(), planes, nextPlaneIndex);
    }

    void BSPSystem::render(const Camera& camera)
    {
        if (!rootNode)
            return;

        Frustum frustum;
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix();
        glm::mat4 viewProjection = projection * view;
        frustum.extractFromMatrix(viewProjection);

        std::vector<Model*> visibleModels;
        rootNode->collectVisibleModels(camera.getPosition(), frustum, visibleModels);

        const BSPPlane* activePlane = rootNode->isLeaf() ? nullptr : &rootNode->plane;

        for (Model* model : visibleModels)
        {
            model->drawWithFrustumAndBSP(frustum, activePlane, camera.getPosition());
        }
    }

    void BSPSystem::clear()
    {
        allModels.clear();
        rootNode = std::make_unique<BSPNode>();
    }
}
