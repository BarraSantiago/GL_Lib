#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <glm/glm.hpp>

#include "Renderer.h"
#include "Mesh.h" 

namespace gllib
{
    class DLLExport ModelImporter
    {
    private:
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

        bool loadModel(const std::string& filePath);
        void assignTexture(size_t meshIndex, const std::string& texturePath,
                           const std::string& textureType = "diffuse");
        const Mesh& getMesh(size_t meshIndex) const;

        size_t getMeshCount() const;
        unsigned int getTexture(size_t meshIndex) const;
        bool hasTexture(size_t meshIndex) const;
        void drawMesh(size_t meshIndex) const;
        void drawAllMeshes() const;
        void cleanup();
    };
}
