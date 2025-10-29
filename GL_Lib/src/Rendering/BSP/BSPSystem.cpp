#include "BSPSystem.h"
#include <algorithm>
#include "Importer/Model.h"
#include "Rendering/Frustum.h"
#include "Rendering/Camera/Camera.h"

namespace gllib {

BSPSystem::BSPSystem() {
    root_ = std::make_unique<BSPNode>();
}

void BSPSystem::addModel(Model* model) {
    if (!model) return;
    if (std::find(models_.begin(), models_.end(), model) == models_.end())
        models_.push_back(model);
}

void BSPSystem::removeModel(Model* model) {
    if (!model) return;
    auto it = std::remove(models_.begin(), models_.end(), model);
    models_.erase(it, models_.end());
}

void BSPSystem::buildBSP(const std::vector<BSPPlane>& planes) {
    // Use the first plane as the active culling plane (simple and explicit per your spec).
    if (!planes.empty()) {
        activePlane_     = planes.front();
        hasActivePlane_  = true;
    } else {
        hasActivePlane_  = false;
    }

    // Keep a minimal node tree for tooling; not required for culling.
    root_ = std::make_unique<BSPNode>(hasActivePlane_ ? activePlane_ : BSPPlane{});
    root_->models.clear();
    for (Model* m : models_) root_->addModel(m);
}

bool BSPSystem::aabbFullyOpposite(const glm::vec3& wMin,
                                  const glm::vec3& wMax,
                                  const BSPPlane&  plane,
                                  bool             cameraInFront)
{
    const glm::vec3 corners[8] = {
        {wMin.x, wMin.y, wMin.z}, {wMax.x, wMin.y, wMin.z},
        {wMin.x, wMax.y, wMin.z}, {wMax.x, wMax.y, wMin.z},
        {wMin.x, wMin.y, wMax.z}, {wMax.x, wMin.y, wMax.z},
        {wMin.x, wMax.y, wMax.z}, {wMax.x, wMax.y, wMax.z}
    };
    // If ANY corner is on the same side as the camera, it is NOT fully opposite.
    for (int i = 0; i < 8; ++i) {
        const bool cornerInFront = plane.isPointInFront(corners[i]);
        if (cornerInFront == cameraInFront) return false;
    }
    return true;
}

void BSPSystem::render(const Camera& camera) {
    // 1) Build frustum from camera
    Frustum frustum(camera.getProjectionMatrix() * camera.getViewMatrix());

    // 2) Determine the side the camera is on (if a plane is active)
    bool cameraInFront = true;
    if (hasActivePlane_) {
        // Use camera world position; if your camera stores it elsewhere, adapt here
        const glm::vec3 camPos = camera.getPosition();
        cameraInFront = activePlane_.isPointInFront(camPos);
    }

    // 3) Per-model strict culling against the ACTIVE plane using the model's HIERARCHICAL AABB
    for (Model* model : models_) {
        if (!model) continue;

        // Keep transforms and hierarchical AABBs fresh
        model->transform.updateTRSAndAABB();

        const glm::vec3 hMin = model->transform.getWorldAABBMin();
        const glm::vec3 hMax = model->transform.getWorldAABBMax();

        // Frustum reject first
        if (!frustum.isAABBInside(hMin, hMax)) continue;

        // Strict BSP reject: if the hierarchical AABB is fully on the opposite side → skip
        if (hasActivePlane_ && aabbFullyOpposite(hMin, hMax, activePlane_, cameraInFront)) continue;

        // Draw normally with frustum-only (BSP culling already decided above)
        model->drawWithFrustum(frustum);
    }
}

void BSPSystem::renderDebug(const Camera& camera, bool drawAABB) {
    if (drawAABB) {
        for (Model* m : models_) {
            if (!m) continue;
            m->drawAllAABBsDebug(camera.getViewMatrix(), camera.getProjectionMatrix());
        }
    }
}

void BSPSystem::clear() {
    models_.clear();
    root_ = std::make_unique<BSPNode>();
    hasActivePlane_ = false;
}

} // namespace gllib
