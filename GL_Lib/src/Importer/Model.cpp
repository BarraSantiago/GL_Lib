#include "Model.h"

#include "Camera.h"
#include "Frustum.h"

namespace gllib
{
    Model::Model(std::string const& path, bool gamma)
    {
        ModelLoader::loadModel(path, meshes, gamma);
    }

    void Model::draw(const Camera& camera)
    {
        Frustum frustum;
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix();
        glm::mat4 viewProjection = projection * view;

        frustum.extractFromMatrix(viewProjection);

        // Check if model's AABB is in frustum
        glm::vec3 worldMin = transform.getWorldAABBMin();
        glm::vec3 worldMax = transform.getWorldAABBMax();

        if (!frustum.isAABBInside(worldMin, worldMax))
        {
            return;
        }

        for (unsigned int i = 0; i < meshes.size(); i++)
        {
            Renderer::drawModel3D(meshes[i].VAO, meshes[i].indices.size(), transform.getTransformMatrix(),
                                  meshes[i].textures);
        }
    }

    void Model::draw()
    {
        // Default draw method, can be overridden if needed
        Camera camera; // Assuming you have a camera instance available
        draw(camera);
    }
}
