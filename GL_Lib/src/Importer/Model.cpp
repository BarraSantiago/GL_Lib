#include "Model.h"

ToToEng::Model::Model(ToToEng::Renderer* renderer, std::string const& path, bool gamma): Entity3D(renderer)
{
    ModelLoader::loadModel(path, meshes, gamma);
}

void ToToEng::Model::draw()
{
    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        renderer->drawModel3D(meshes[i].VAO, meshes[i].indices.size(), transform.getTransformMatrix(),
                              meshes[i].textures);
    }
}
