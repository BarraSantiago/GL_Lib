#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>

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

    const Mesh& getMesh(size_t meshIndex) const;
    void drawMesh(size_t meshIndex) const;
    void drawAllMeshes() const;
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    ~Mesh();

private:
    unsigned int VBO, EBO;
    void setupMesh();
};