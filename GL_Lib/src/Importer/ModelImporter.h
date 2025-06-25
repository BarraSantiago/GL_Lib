#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <string>
#include <memory>
#include <map>

#include "renderer.h"

namespace gllib
{
    class DLLExport ModelImporter
    {
    private:
        struct Texture
        {
            unsigned int id;
            std::string type;
            std::string path;
        };

        struct Vertex
        {
            glm::vec3 position;
            glm::vec3 normal;
            glm::vec2 texCoords;
        };

        struct Mesh
        {
            std::vector<Vertex> vertices;
            std::vector<unsigned int> indices;
            std::vector<Texture> textures;
            gllib::RenderData renderData;
        };

        std::vector<Mesh> meshes;
        std::string directory;
        std::vector<Texture> textures_loaded;

        void processNode(aiNode* node, const aiScene* scene);
        Mesh processMesh(aiMesh* mesh, const aiScene* scene);
        std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
        unsigned int textureFromFile(const char* path, const std::string& directory, bool gamma = false);

    public:
        ModelImporter();
        ~ModelImporter();

        // Load a model from file
        bool loadModel(const std::string& filePath);

        // Get number of meshes in the model
        size_t getMeshCount() const;

        // Get RenderData for a specific mesh
        gllib::RenderData getRenderData(size_t meshIndex) const;

        // Get indices count for a specific mesh (needed for drawElements)
        size_t getIndicesCount(size_t meshIndex) const;

        // Get texture ID for a mesh (first diffuse texture)
        unsigned int getTexture(size_t meshIndex) const;

        // Does the mesh have a diffuse texture?
        bool hasTexture(size_t meshIndex) const;

        // Clean up all meshes
        void cleanup();
    };
}
