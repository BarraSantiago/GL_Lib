#pragma once
#include <vector>

#include "Rendering/Camera/Camera.h"
#include "Rendering/Frustum.h"
#include "Mesh.h"
#include "ModelLoader.h"
#include "Entities/Entity3D.h"

namespace gllib
{
    struct BSPPlane;

    class DLLExport Model : public Entity3D
    {
    private:
        void drawHierarchical(const Frustum& frustum);
        void drawChildTransform(Transform* childTransform, const Frustum& frustum);
        void drawTransformAABB(Transform* t, const glm::mat4& view, const glm::mat4& projection);
        bool subtreeHasAnyOnCameraSide(const Transform* t, const BSPPlane* plane, bool cameraInFront);
        void drawNodeWithBSP(Transform* t, const Frustum& frustum, const BSPPlane* bspPlane, bool cameraInFront);
        
        std::vector<Transform*> allTransforms;
        static std::unordered_map<Transform*, Model*> transformToModelMap;

        bool isPlaneModel_ = false;
        unsigned int aabbVAO = 0;
        unsigned int aabbVBO = 0;
        bool aabbInitialized = false;
        void initializeAABBVisualization();

        std::unordered_map<Transform*, Material*> transformMaterials;
        
    public:
        void setMaterialForTransform(Transform* transform, Material* material);
        Material* getMaterialForTransform(Transform* transform);
        std::vector<Mesh> meshes;
        Model(std::string const& path, bool gamma);
        ~Model();
        
        static bool isPlaneModel(const std::string& path);
        bool isFromPlanesFolder() const { return isPlaneModel_; }
        void draw(const Camera& camera);
        void draw() override;
        void drawAABBDebug(const glm::mat4& view, const glm::mat4& projection);
        void drawAllAABBsDebug(const glm::mat4& view, const glm::mat4& projection);
        void drawWithFrustum(const Frustum& frustum);
        void drawFrustumAndBSP(const Frustum& frustum, const BSPPlane* bspPlane, const glm::vec3& cameraPos);
        static void registerModel(Transform* transform, Model* model);
        static void unregisterModel(Transform* transform);
        static Model* getModelFromTransform(Transform* transform);
    };
}
