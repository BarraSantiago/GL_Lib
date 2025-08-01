#pragma once

#include "deps.h"
#include "renderer.h"
#include "transform.h"
#include "transform.h"

namespace gllib
{
    struct Vector2
    {
        float x;
        float y;
    };

    class DLLExport Entity2
    {
    protected:
        float* vertices;
        unsigned int* indices;
        unsigned int VBO;
        unsigned int IBO;
        unsigned int VAO;
        unsigned int id;
        unsigned int indexQty;
        unsigned int vertexQty;
        float* positions;
        float* colors;
        float* textureCoords;

        virtual void updateVao();

    public:
        glm::vec4 color;
        bool active;
        Transform transform;

        Entity2();
        virtual ~Entity2();

        virtual void genBuffers();
        virtual void deleteBuffers();
        virtual void update();
        virtual void draw() = 0;

        glm::vec4 getColor();

        void setColor(glm::vec4 color);
    };
}
