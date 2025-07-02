#include "Model.h"

gllib::Model::Model(std::string const& path, bool gamma): Entity3D()
{
    ModelLoader::loadModel(path, meshes, gamma);
}

void gllib::Model::draw()
{
    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        Renderer::drawModel3D(meshes[i].VAO, meshes[i].indices.size(), transform.getTransformMatrix(), meshes[i].textures);
    }
}

