#pragma once

#include "Rendering/renderer.h"
#include "Math/transform.h"
#include "entity.h"

namespace gllib {

    class DLLExport Shape : public Entity {
    private:
        RenderData renderData;
        unsigned int indexSize;
        unsigned int shaderProgram;
        glm::mat4 cachedModelMatrix;

    protected:
        void alignVertex(float* vertexData, int vertexCount, int vertexStride);
        void setRenderData(const float vertexData[], int vertexDataSize, const int index[], int indexSize);
        void internalDraw();
        unsigned int getActiveShaderProgram() const;
        void updateModelMatrix() override;

    public:
        Shape(Vector3 translation, Vector3 rotation, Vector3 scale);
        Shape(Transform transform);
        virtual ~Shape();

        unsigned int getShaderProgram() const;
        void setShaderProgram(unsigned int shaderProgram);

        virtual void draw() = 0;
    };
}
