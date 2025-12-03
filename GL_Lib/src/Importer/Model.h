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
        std::vector<Transform*> allTransforms;
        std::unordered_map<Transform*, Material*> transformMaterials;
        static std::unordered_map<Transform*, Model*> transformToModelMap;

        bool isPlaneModel_ = false;
        unsigned int aabbVAO = 0;
        unsigned int aabbVBO = 0;
        bool aabbInitialized = false;

        void drawHierarchical(const Frustum& frustum);
        void drawChildTransform(Transform* childTransform, const Frustum& frustum);
        void drawTransformAABB(Transform* t, const glm::mat4& view, const glm::mat4& projection);
        bool subtreeHasAnyOnCameraSide(const Transform* t, const BSPPlane* plane, bool cameraInFront);
        void drawNodeWithBSP(Transform* t, const Frustum& frustum, const BSPPlane* bspPlane, bool cameraInFront);
        void initializeAABBVisualization();

    public:
        std::vector<Mesh> meshes;
        
        Model(std::string const& path, bool gamma);
        ~Model();

        void draw() override;
        static bool isPlaneModel(const std::string& path);
        bool isFromPlanesFolder() const { return isPlaneModel_; }
        void setMaterialForTransform(Transform* transform, Material* material);
        void draw(const Camera& camera);
        void drawAABBDebug(const glm::mat4& view, const glm::mat4& projection);
        void drawAllAABBsDebug(const glm::mat4& view, const glm::mat4& projection);
        void drawWithFrustum(const Frustum& frustum);
        void drawFrustumAndBSP(const Frustum& frustum, const BSPPlane* bspPlane, const glm::vec3& cameraPos);
        static void registerModel(Transform* transform, Model* model);
        static void unregisterModel(Transform* transform);
        static Model* getModelFromTransform(Transform* transform);
        static const std::list<Model*>& getLoadedModels();
        Material* getMaterialForTransform(Transform* transform);
        std::vector<BSPPlane> createBSPPlanesFromNodes();
        void searchForPlaneNodes(Transform* t, const std::string& nodeName, std::vector<BSPPlane>& planes);
    };

    static std::list<Model*> loadedModels;
}
