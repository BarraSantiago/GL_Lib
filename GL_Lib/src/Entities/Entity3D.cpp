#include "Entity3D.h"

#include "BSP/BSPSystem.h"

namespace gllib
{
    Entity3D::Entity3D() : Entity2()
    {
        material = new Material();
        
        vertexQty = 24;
        indexQty = 36;
        id = 1;

        color = glm::vec4(1.0f, 0.5f, 0.31f, 1.f);

        positions = new float[vertexQty * 3]
        {
            -.5f, -.5f, -.5f,
            .5f, -.5f, -.5f,
            -.5f, .5f, -.5f,
            .5f, .5f, -.5f,

            .5f, -.5f, -.5f,
            .5f, -.5f, .5f,
            .5f, .5f, -.5f,
            .5f, .5f, .5f,

            -.5f, -.5f, -.5f,
            -.5f, .5f, -.5f,
            -.5f, -.5f, .5f,
            -.5f, .5f, .5f,

            -.5f, .5f, -.5f,
            .5f, .5f, -.5f,
            -.5f, .5f, .5f,
            .5f, .5f, .5f,

            -.5f, -.5f, -.5f,
            .5f, -.5f, -.5f,
            -.5f, -.5f, .5f,
            .5f, -.5f, .5f,

            -.5f, -.5f, .5f,
            .5f, -.5f, .5f,
            -.5f, .5f, .5f,
            .5f, .5f, .5f
        };

        normals = new float[vertexQty * 3]
        {
            0.f, 0.f, -1.f,
            0.f, 0.f, -1.f,
            0.f, 0.f, -1.f,
            0.f, 0.f, -1.f,

            1.f, 0.f, 0.f,
            1.f, 0.f, 0.f,
            1.f, 0.f, 0.f,
            1.f, 0.f, 0.f,

            -1.f, 0.f, 0.f,
            -1.f, 0.f, 0.f,
            -1.f, 0.f, 0.f,
            -1.f, 0.f, 0.f,

            0.f, 1.f, 0.f,
            0.f, 1.f, 0.f,
            0.f, 1.f, 0.f,
            0.f, 1.f, 0.f,

            0.f, -1.f, 0.f,
            0.f, -1.f, 0.f,
            0.f, -1.f, 0.f,
            0.f, -1.f, 0.f,

            0.f, 0.f, 1.f,
            0.f, 0.f, 1.f,
            0.f, 0.f, 1.f,
            0.f, 0.f, 1.f
        };
        
        textureCoords = new float[vertexQty * 2]
        {
            0.0f, 0.0f,
            1.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f,

            0.0f, 0.0f,
            1.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f,

            0.0f, 0.0f,
            1.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f,

            0.0f, 0.0f,
            1.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f,

            0.0f, 0.0f,
            1.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f,

            0.0f, 0.0f,
            1.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f
        };

        indices = new unsigned int[indexQty]
        {
            0, 1, 2,
            1, 2, 3,

            4, 5, 6,
            5, 6, 7,

            8, 9, 10,
            9, 10, 11,

            12, 13, 14,
            13, 14, 15,

            16, 17, 18,
            17, 18, 19,

            20, 21, 22,
            21, 22, 23
        };

        updateVao();
    }

    Entity3D::~Entity3D()
    {
        delete normals;
        delete material;
    }

    void Entity3D::draw()
    {
        Renderer::drawEntity3D(VAO, indexQty, *material, transform.getTransformMatrix());
    }

    void Entity3D::updateVao()
    {
        delete vertices;

        vertices = new float[vertexQty * 8];

        for (unsigned int i = 0; i < vertexQty; i++)
        {
            vertices[8 * i] = positions[i * 3];
            vertices[8 * i + 1] = positions[i * 3 + 1];
            vertices[8 * i + 2] = positions[i * 3 + 2];

            vertices[8 * i + 3] = normals[i * 3];
            vertices[8 * i + 4] = normals[i * 3 + 1];
            vertices[8 * i + 5] = normals[i * 3 + 2];

            vertices[8 * i + 6] = textureCoords[i * 2];
            vertices[8 * i + 7] = textureCoords[i * 2 + 1];
        }

        genBuffers();
    }

    void Entity3D::setMaterial(Material* material)
    {
        this->material = material;
    }

    void Entity3D::makeBSPPlane(BSPSystem* bspSystem)
    {
        if (!bspSystem) return;
        
        BSPPlane plane = createBSPPlane();
        bspSystem->addPlane(plane);
    }

    BSPPlane Entity3D::createBSPPlane() const
    {
        BSPPlane plane;
        
        // Transform the local +Z normal by the rotation
        glm::vec3 localNormal(0.0f, 0.0f, 1.0f);
        glm::mat3 rotationMatrix = glm::toMat3(transform.rotationQuat);
        plane.normal = glm::normalize(rotationMatrix * localNormal);
        
        // Distance from origin to plane along normal
        glm::vec3 pointOnPlane = transform.position;
        plane.distance = -glm::dot(plane.normal, pointOnPlane);
        
        return plane;
    }
}
