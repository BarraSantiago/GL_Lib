#pragma once
#include <string>
#include <vector>
#include <glm.hpp>
#include "deps.h"

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};

struct Texture
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

    // Remove these lines - they don't belong in Mesh class:
    // const Mesh& getMesh(size_t meshIndex) const;
    // void drawMesh(size_t meshIndex) const;
    // void drawAllMeshes() const;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    ~Mesh();

private:
    unsigned int VBO, EBO;
    void setupMesh();
};
