#pragma once
#include <string>
#include <vector>
#include <glm.hpp>
#include "deps.h"
#include "renderer.h"
// Forward declarations to avoid circular dependencies
namespace gllib
{
    struct RenderData;
    class Renderer;
}

namespace gllib
{
    struct DLLExport Vertex
    {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
        glm::vec3 Tangent;
        glm::vec3 Bitangent;
    };

    struct DLLExport Texture
    {
        unsigned int id;
        std::string type;
        std::string path;
    };

    class DLLExport Mesh
    {
    public:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;
        unsigned int VAO;
        RenderData renderData;
        GLsizei indexSize;

        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
        ~Mesh();

    private:
        unsigned int VBO, EBO;
        void setupMesh();
        void createRenderData();
    };
}