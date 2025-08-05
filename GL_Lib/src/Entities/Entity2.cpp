#include "Entity2.h"

#include "transform.h"

namespace gllib
{
    Entity2::Entity2()
    {

        color = {1, 1, 1, 1};

        positions = new float();
        colors = new float();
        textureCoords = new float();
        indices = new unsigned int();
        vertices = new float();

        transform = Transform();
        transform.position = glm::vec3(0.0f);
        transform.scale = glm::vec3(1.0f);
        transform.rotationQuat = Quaternion{1.0f, 0.0f, 0.0f, 0.0f};
    }

    Entity2::~Entity2()
    {
        delete positions;
        delete colors;
        delete vertices;
        delete indices;

        Entity2::deleteBuffers();
    }

    void Entity2::genBuffers()
    {
        Renderer::genVertexBuffer(VBO, VAO, vertices, id, vertexQty);
        Renderer::genIndexBuffer(IBO, indices, id, indexQty);
    }

    void Entity2::deleteBuffers()
    {
        Renderer::deleteBuffers(VBO, IBO, VAO, id);
    }

    void Entity2::update()
    {
    }

    void Entity2::setColor(glm::vec4 color)
    {
        this->color = color;
    }

    glm::vec4 Entity2::getColor()
    {
        return color;
    }

    void Entity2::updateVao()
    {
        delete vertices;

        vertices = new float[vertexQty * 9];

        for (unsigned int i = 0; i < vertexQty; i++)
        {
            vertices[9 * i] = positions[i * 3];
            vertices[9 * i + 1] = positions[i * 3 + 1];
            vertices[9 * i + 2] = positions[i * 3 + 2];

            vertices[9 * i + 3] = colors[i * 4];
            vertices[9 * i + 4] = colors[i * 4 + 1];
            vertices[9 * i + 5] = colors[i * 4 + 2];
            vertices[9 * i + 6] = colors[i * 4 + 3];

            vertices[9 * i + 7] = textureCoords[i * 2];
            vertices[9 * i + 8] = textureCoords[i * 2 + 1];
        }

        genBuffers();
    }
}
