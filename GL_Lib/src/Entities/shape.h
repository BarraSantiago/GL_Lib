#pragma once

#include "entity.h"

namespace gllib {

    class DLLExport Shape : public Entity {
    private:
        RenderData renderData;
        unsigned int indexSize;

    protected:
        void alignVertex(float* vertexData, int vertexCount, int vertexStride);
        void setRenderData(const float vertexData[], int vertexDataSize, const int index[], int indexSize);
        void internalDraw();

    public:
        Shape(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale);
        Shape(Transform transform);
        virtual ~Shape();

        virtual void draw() = 0;
    };
}
