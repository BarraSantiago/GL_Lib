#pragma once
#include <string>
#include <vector>

#include "Mesh.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

namespace gllib
{
    static class DLLExport ModelLoader
    {
    public:
        static std::vector<Texture> textures_loaded;
        static std::string directory;
        static bool gammaCorrection;

        static void loadModel(std::string const& path, std::vector<Mesh>& meshes, bool gamma = false);

    private:
        static void processNode(aiNode* node, const aiScene* scene, std::vector<Mesh>& meshes, bool gamma = false);
        static Mesh processMesh(aiMesh* mesh, const aiScene* scene, std::vector<Mesh>& meshes, bool gamma = false);

        static std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName,
                                                         bool gamma = false);
    };

    static unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma);
}
