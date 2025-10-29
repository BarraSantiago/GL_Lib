#pragma once
#include <vector>

#include "Rendering/Camera/Camera.h"
#include "Rendering/Frustum.h"
#include "Mesh.h"
#include "ModelLoader.h"
#include "Entities/Entity3D.h"

namespace gllib
{
    class DLLExport Model : public Entity3D
    {
    private:
        std::vector<Transform*> allTransforms;
        
        void drawHierarchical(const Frustum& frustum);
        void drawChildTransform(Transform* childTransform, const Frustum& frustum);
        static std::unordered_map<Transform*, Model*> transformToModelMap;

    public:
        std::vector<Mesh> meshes;
        Model(std::string const& path, bool gamma);
        ~Model();

        void draw(const Camera& camera);
        void draw() override;
        void drawWithFrustum(const Frustum& frustum);
        
        static void registerModel(Transform* transform, Model* model);
        static void unregisterModel(Transform* transform);
        static Model* getModelFromTransform(Transform* transform);
    };
}
