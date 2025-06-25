#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <string>
#include <memory>

#include "renderer.h"

namespace gllib
{
    class ModelImporter
    {
    private:
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
            gllib::RenderData renderData;
        };

        std::vector<Mesh> meshes;
        std::string directory;

        void processNode(aiNode* node, const aiScene* scene);
        Mesh processMesh(aiMesh* mesh, const aiScene* scene);

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

        // Clean up all meshes
        void cleanup();
    };
}
