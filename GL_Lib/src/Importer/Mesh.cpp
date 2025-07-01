#include "Mesh.h"
#include "Model.h"

namespace gllib
{
    Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        this->indexSize = static_cast<GLsizei>(indices.size());

        // Initialize OpenGL buffer IDs
        VAO = 0;
        VBO = 0;
        EBO = 0;

        // Set up the vertex buffers and attribute pointers
        setupMesh();
        createRenderData();
    }

    Mesh::~Mesh()
    {
        // Clean up OpenGL buffers
        if (VAO != 0)
        {
            glDeleteVertexArrays(1, &VAO);
            VAO = 0;
        }
        if (VBO != 0)
        {
            glDeleteBuffers(1, &VBO);
            VBO = 0;
        }
        if (EBO != 0)
        {
            glDeleteBuffers(1, &EBO);
            EBO = 0;
        }

        // Clean up render data
        if (renderData.VAO != 0)
        {
            gllib::Renderer::destroyRenderData(renderData);
        }
    }

    void Mesh::setupMesh()
    {
        // Validate data before setting up buffers
        if (vertices.empty() || indices.empty())
        {
            return; // Don't set up empty meshes
        }

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // Set vertex attributes to match your vertex shader
        // Position attribute (location = 0)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));

        // Normal attribute (location = 1)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

        // Texture coordinate attribute (location = 2)
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

        glBindVertexArray(0);
    }

    void Mesh::createRenderData()
    {
        if (vertices.empty() || indices.empty())
        {
            return;
        }

        // Convert vertex data to float array format expected by Renderer
        std::vector<float> vertexData;
        for (const auto& vertex : vertices)
        {
            // Position
            vertexData.push_back(vertex.Position.x);
            vertexData.push_back(vertex.Position.y);
            vertexData.push_back(vertex.Position.z);

            // Color (default white)
            vertexData.push_back(1.0f);
            vertexData.push_back(1.0f);
            vertexData.push_back(1.0f);
            vertexData.push_back(1.0f);

            // Texture coordinates
            vertexData.push_back(vertex.TexCoords.x);
            vertexData.push_back(vertex.TexCoords.y);
        }

        // Convert indices to int array
        std::vector<int> indexData(indices.begin(), indices.end());

        // Create render data using Renderer
        renderData = gllib::Renderer::createRenderData(
            vertexData.data(),
            static_cast<GLsizei>(vertexData.size()),
            indexData.data(),
            static_cast<GLsizei>(indexData.size())
        );
    }
}
