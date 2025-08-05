#pragma once
#include <vector>


#include "Shader.h"
#include "Math/transform.h"

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
    unsigned int VBO, EBO;
    glm::vec3 minAABB;
    glm::vec3 maxAABB;
    gllib::Transform* associatedTransform = nullptr;
    
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    
private:

    void setupMesh();
};
