#include "BSPNode.h"
#include "Importer/Model.h"
#include "Rendering/Frustum.h"
#include "Rendering/Camera/Camera.h"

namespace gllib {

    void BSPNode::addModel(Model* model) {
        if (model) models.push_back(model);
    }

    void BSPNode::collectVisibleModels(const glm::vec3&,const Frustum& frustum, std::vector<Model*>& out)
    {
        if (isLeaf()) {
            for (Model* m : models) {
                if (!m) continue;
                const glm::vec3 mn = m->transform.getWorldAABBMin();
                const glm::vec3 mx = m->transform.getWorldAABBMax();
                if (frustum.isAABBInside(mn, mx)) out.push_back(m);
            }
            return;
        }
        if (frontChild) frontChild->collectVisibleModels({}, frustum, out);
        if (backChild)  backChild ->collectVisibleModels({}, frustum, out);
    }
} 
