#include "BSPNode.h"
#include "Importer/Model.h"
#include "Rendering/Frustum.h"
#include "Rendering/Camera/Camera.h"

namespace gllib {

    void BSPNode::addModel(Model* model) {
        // The rebuilt pipeline culls in BSPSystem, not per-node. Keep membership for tooling only.
        if (model) models.push_back(model);
    }

    void BSPNode::collectVisibleModels(const glm::vec3& /*cameraPos*/,
                                       const Frustum&   frustum,
                                       std::vector<Model*>& out)
    {
        // Simple leaf pass-thru for tooling; visibility is decided in BSPSystem now.
        if (isLeaf()) {
            for (Model* m : models) {
                if (!m) continue;
                const auto mn = m->transform.getWorldAABBMin();
                const auto mx = m->transform.getWorldAABBMax();
                if (frustum.isAABBInside(mn, mx)) out.push_back(m);
            }
            return;
        }
        if (frontChild) frontChild->collectVisibleModels({}, frustum, out);
        if (backChild)  backChild ->collectVisibleModels({}, frustum, out);
    }

    void BSPNode::render(const Camera& /*camera*/, const Frustum& /*frustum*/) {
        // Intentionally empty in the rebuilt flow. Keep for API compatibility or add your debug draw here.
    }

} // namespace gllib
