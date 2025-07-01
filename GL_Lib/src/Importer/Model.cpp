#include "Model.h"

namespace gllib
{
    Model::Model(gllib::Renderer* renderer) : renderer(renderer)
    {
    }

    Model::~Model()
    {
        cleanup();
    }

    bool Model::loadModel(const std::string& path, bool gamma)
    {
        cleanup();
        meshes.clear();

        // You'll need to implement actual model loading here
        // For now, this is a placeholder that returns false
        // You could use your ModelImporter class here:

        // ModelImporter importer;
        // if (!importer.loadModel(path))
        //     return false;

        // for (size_t i = 0; i < importer.getMeshCount(); i++)
        // {
        //     meshes.push_back(importer.getMesh(i));
        // }

        return false; // Placeholder - implement actual loading
    }

    void Model::draw()
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
        {
            // Bind textures
            for (unsigned int j = 0; j < meshes[i].textures.size(); j++)
            {
                glActiveTexture(GL_TEXTURE0 + j);
                glBindTexture(GL_TEXTURE_2D, meshes[i].textures[j].id);
            }

            // Draw mesh using its VAO
            glBindVertexArray(meshes[i].VAO);
            glDrawElements(GL_TRIANGLES, meshes[i].indices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
    }

    void Model::cleanup()
    {
        // Mesh destructors will handle VAO/VBO/EBO cleanup automatically
        // Just clear the vector
        meshes.clear();
    }
}
