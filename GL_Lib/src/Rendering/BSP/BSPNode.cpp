#include "BSPNode.h"
#include "Camera.h"
#include "Frustum.h"
#include "Importer/Model.h"
#include <iostream>

namespace gllib
{
    void BSPNode::addModel(Model* model)
    {
        if (isLeaf())
        {
            models.push_back(model);
            return;
        }

        glm::vec3 modelCenter = (model->transform.getWorldAABBMin() + model->transform.getWorldAABBMax()) * 0.5f;

        if (plane.isPointInFront(modelCenter))
        {
            if (!frontChild)
                frontChild = std::make_unique<BSPNode>();
            frontChild->addModel(model);
        }
        else
        {
            if (!backChild)
                backChild = std::make_unique<BSPNode>();
            backChild->addModel(model);
        }
    }

    void BSPNode::collectVisibleModels(const glm::vec3& cameraPos, const Frustum& frustum,
                                       std::vector<Model*>& visibleModels)
    {
        if (isLeaf())
        {
            for (Model* model : models)
            {
                if (model)
                {
                    // Quick BSP-level AABB check before adding to queue
                    glm::vec3 aabbMin = model->transform.getWorldAABBMin();
                    glm::vec3 aabbMax = model->transform.getWorldAABBMax();

                    if (frustum.isAABBInside(aabbMin, aabbMax))
                    {
                        visibleModels.push_back(model);
                    }
                }
            }
            return;
        }

        bool cameraInFront = plane.isPointInFront(cameraPos);

        // Render back-to-front for proper transparency support
        if (cameraInFront)
        {
            if (backChild)
                backChild->collectVisibleModels(cameraPos, frustum, visibleModels);
            if (frontChild)
                frontChild->collectVisibleModels(cameraPos, frustum, visibleModels);
        }
        else
        {
            if (frontChild)
                frontChild->collectVisibleModels(cameraPos, frustum, visibleModels);
            if (backChild)
                backChild->collectVisibleModels(cameraPos, frustum, visibleModels);
        }
    }
}
