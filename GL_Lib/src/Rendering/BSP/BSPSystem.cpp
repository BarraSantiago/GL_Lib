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

    void BSPSystem::buildBSP(const std::vector<BSPPlane>& planes)
    {
        if (!planes.empty())
        {
            activePlane_ = planes.front();
            hasActivePlane_ = true;
        }
        else
        {
            hasActivePlane_ = false;
        }

        root_ = std::make_unique<BSPNode>(hasActivePlane_ ? activePlane_ : BSPPlane{});
        root_->models.clear();
        for (Model* m : models_) root_->addModel(m);
    }

    bool BSPSystem::aabbFullyOpposite(const glm::vec3& wMin,const glm::vec3& wMax,const BSPPlane& plane,bool cameraInFront)
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

        bool cameraInFront = true;
        if (hasActivePlane_)
        {
            const glm::vec3 camPos = camera.getPosition();
            cameraInFront = activePlane_.isPointInFront(camPos);
        }

        for (Model* model : models_)
        {
            if (!model) continue;

            model->transform.updateTRSAndAABB();

            const glm::vec3 modelMin = model->transform.getWorldAABBMin();
            const glm::vec3 modelMax = model->transform.getWorldAABBMax();

            if (!frustum.isAABBInside(modelMin, modelMax)) continue;

            if (hasActivePlane_ && aabbFullyOpposite(modelMin, modelMax, activePlane_, cameraInFront)) continue;

            model->drawFrustumAndBSP(frustum, hasActivePlane_ ? &activePlane_ : nullptr, camera.getPosition());
        }
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
    
    void BSPSystem::clear()
    {
        models_.clear();
        root_ = std::make_unique<BSPNode>();
        hasActivePlane_ = false;
    }
}