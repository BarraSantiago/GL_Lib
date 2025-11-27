#include "BSPSystem.h"
#include <algorithm>
#include "Importer/Model.h"
#include "Rendering/Frustum.h"
#include "Rendering/Camera/Camera.h"

namespace gllib
{
    BSPSystem::BSPSystem()
    {
        root_ = std::make_unique<BSPNode>();
    }

    void BSPSystem::addModel(Model* model)
    {
        if (!model) return;
        models_.push_back(model);
    }

    void BSPSystem::removeModel(Model* model)
    {
        if (!model) return;
        std::vector<Model*>::iterator it = std::remove(models_.begin(), models_.end(), model);
        models_.erase(it, models_.end());
    }

    bool BSPSystem::aabbFullyOpposite(const glm::vec3& wMin, const glm::vec3& wMax, const BSPPlane& plane,
                                      bool cameraInFront)
    {
        const glm::vec3 corners[8] = {
            {wMin.x, wMin.y, wMin.z}, {wMax.x, wMin.y, wMin.z},
            {wMin.x, wMax.y, wMin.z}, {wMax.x, wMax.y, wMin.z},
            {wMin.x, wMin.y, wMax.z}, {wMax.x, wMin.y, wMax.z},
            {wMin.x, wMax.y, wMax.z}, {wMax.x, wMax.y, wMax.z}
        };
        for (int i = 0; i < 8; ++i)
        {
            const bool cornerInFront = plane.isPointInFront(corners[i]);
            if (cornerInFront == cameraInFront) return false;
        }
        return true;
    }

    void BSPSystem::render(const Camera& camera)
    {
        Frustum frustum(camera.getProjectionMatrix() * camera.getViewMatrix());

        if (!hasActivePlane_)
        {
            // No BSP tree - render all models
            for (Model* model : models_)
            {
                if (!model) continue;
                model->transform.updateTRSAndAABB();

                const glm::vec3 modelMin = model->transform.getWorldAABBMin();
                const glm::vec3 modelMax = model->transform.getWorldAABBMax();

                if (!frustum.isAABBInside(modelMin, modelMax)) continue;

                model->drawFrustumAndBSP(frustum, nullptr, camera.getPosition());
            }
            return;
        }

        // Traverse BSP tree front-to-back
        renderNode(root_.get(), frustum, camera.getPosition());
    }

    void BSPSystem::renderDebug(const Camera& camera, bool drawAABB)
    {
        if (drawAABB)
        {
            for (Model* m : models_)
            {
                if (!m) continue;
                m->drawAllAABBsDebug(camera.getViewMatrix(), camera.getProjectionMatrix());
            }
        }
    }

    void BSPSystem::addPlane(const BSPPlane& plane)
    {
        planes_.push_back(plane);
    }

    void BSPSystem::clearPlanes()
    {
        planes_.clear();
    }

    void BSPSystem::buildBSP()
    {
        buildBSP(planes_);
    }

    void BSPSystem::buildBSP(const std::vector<BSPPlane>& planes)
    {
        if (planes.empty())
        {
            hasActivePlane_ = false;
            root_ = std::make_unique<BSPNode>();
            for (Model* m : models_) root_->addModel(m);
            return;
        }

        hasActivePlane_ = true;
        root_ = std::make_unique<BSPNode>(planes[0]);

        // Build recursive BSP tree
        std::vector<BSPPlane> remainingPlanes(planes.begin() + 1, planes.end());
        buildBSPTree(root_.get(), remainingPlanes, models_);
    }


    void BSPSystem::buildBSPTree(BSPNode* node, const std::vector<BSPPlane>& remainingPlanes,
                                 const std::vector<Model*>& models)
    {
        if (remainingPlanes.empty())
        {
            // Leaf node - add all models
            for (Model* m : models) node->addModel(m);
            return;
        }

        const BSPPlane& currentPlane = node->plane;

        std::vector<Model*> frontModels, backModels;

        // Partition models based on current plane
        for (Model* m : models)
        {
            if (!m) continue;

            const glm::vec3 modelMin = m->transform.getWorldAABBMin();
            const glm::vec3 modelMax = m->transform.getWorldAABBMax();

            // Check all 8 corners
            bool anyFront = false, anyBack = false;
            const glm::vec3 corners[8] = {
                {modelMin.x, modelMin.y, modelMin.z}, {modelMax.x, modelMin.y, modelMin.z},
                {modelMin.x, modelMax.y, modelMin.z}, {modelMax.x, modelMax.y, modelMin.z},
                {modelMin.x, modelMin.y, modelMax.z}, {modelMax.x, modelMin.y, modelMax.z},
                {modelMin.x, modelMax.y, modelMax.z}, {modelMax.x, modelMax.y, modelMax.z}
            };

            for (int i = 0; i < 8; ++i)
            {
                if (currentPlane.isPointInFront(corners[i]))
                    anyFront = true;
                else
                    anyBack = true;
            }

            // Model spans both sides or is on one side
            if (anyFront) frontModels.push_back(m);
            if (anyBack) backModels.push_back(m);
        }

        // Create child nodes with next plane
        if (!frontModels.empty())
        {
            node->frontChild = std::make_unique<BSPNode>(remainingPlanes[0]);
            std::vector<BSPPlane> nextPlanes(remainingPlanes.begin() + 1, remainingPlanes.end());
            buildBSPTree(node->frontChild.get(), nextPlanes, frontModels);
        }

        if (!backModels.empty())
        {
            node->backChild = std::make_unique<BSPNode>(remainingPlanes[0]);
            std::vector<BSPPlane> nextPlanes(remainingPlanes.begin() + 1, remainingPlanes.end());
            buildBSPTree(node->backChild.get(), nextPlanes, backModels);
        }
    }
    
    void BSPSystem::renderNode(BSPNode* node, const Frustum& frustum, const glm::vec3& cameraPos)
    {
        if (!node) return;
        
        const bool cameraInFront = node->plane.isPointInFront(cameraPos);
        
        // Render front-to-back
        BSPNode* nearNode = cameraInFront ? node->frontChild.get() : node->backChild.get();
        BSPNode* farNode = cameraInFront ? node->backChild.get() : node->frontChild.get();
        
        // Render near side first
        if (nearNode) renderNode(nearNode, frustum, cameraPos);
        
        // Render models at this node - check against ALL planes
        for (Model* model : node->models)
        {
            if (!model) continue;
            
            model->transform.updateTRSAndAABB();
            
            const glm::vec3 modelMin = model->transform.getWorldAABBMin();
            const glm::vec3 modelMax = model->transform.getWorldAABBMax();
            
            if (!frustum.isAABBInside(modelMin, modelMax)) continue;
            
            // Check if model is on camera side of ALL planes
            if (!isModelOnCameraSideOfAllPlanes(modelMin, modelMax, cameraPos)) continue;
            
            model->drawFrustumAndBSP(frustum, &node->plane, cameraPos);
        }
        
        // Render far side last
        if (farNode) renderNode(farNode, frustum, cameraPos);
    }
    
    bool BSPSystem::isModelOnCameraSideOfAllPlanes(const glm::vec3& modelMin, const glm::vec3& modelMax,
                                                    const glm::vec3& cameraPos) const
    {
        if (planes_.empty()) return true;
    
        const glm::vec3 corners[8] = {
            {modelMin.x, modelMin.y, modelMin.z}, {modelMax.x, modelMin.y, modelMin.z},
            {modelMin.x, modelMax.y, modelMin.z}, {modelMax.x, modelMax.y, modelMin.z},
            {modelMin.x, modelMin.y, modelMax.z}, {modelMax.x, modelMin.y, modelMax.z},
            {modelMin.x, modelMax.y, modelMax.z}, {modelMax.x, modelMax.y, modelMax.z}
        };
    
        // Check each plane
        for (const BSPPlane& plane : planes_)
        {
            const bool cameraInFront = plane.isPointInFront(cameraPos);
            
            // Check if ANY corner of the model is on the same side as camera
            bool anyCornerOnCameraSide = false;
            for (int i = 0; i < 8; ++i)
            {
                if (plane.isPointInFront(corners[i]) == cameraInFront)
                {
                    anyCornerOnCameraSide = true;
                    break;
                }
            }
            
            // If NO corners are on camera side for this plane, reject model
            if (!anyCornerOnCameraSide)
                return false;
        }
        
        // Model passes all plane tests
        return true;
    }


    void BSPSystem::clear()
    {
        models_.clear();
        root_ = std::make_unique<BSPNode>();
        hasActivePlane_ = false;
    }
}
