#include "shape.h"

#include <iostream>

using namespace gllib;
using namespace std;

Shape::Shape(Vector3 translation, Vector3 rotation, Vector3 scale) : 
    Entity(translation, rotation, scale) {
    renderData.VAO = 0;
    renderData.VBO = 0;
    renderData.EBO = 0;
    indexSize = 0;
    cout << "Created shape.\n";
}

Shape::Shape(Transform transform) :
    Entity(transform){
    renderData.VAO = 0;
    renderData.VBO = 0;
    renderData.EBO = 0;
    indexSize = 0;
    cout << "Created shape.\n";
}

Shape::~Shape() {
    cout << "Destroyed shape.\n";
    // Destroy the render data to free up vram
    Renderer::destroyRenderData(renderData);
}

// Protected

void Shape::alignVertex(float* vertexData, int vertexCount, int vertexStride) {
    // Calculate the centroid
    float centerX = 0.0f, centerY = 0.0f, centerZ = 0.0f;

    for (int i = 0; i < vertexCount; ++i) {
        centerX += vertexData[i * vertexStride];
        centerY += vertexData[i * vertexStride + 1];
        centerZ += vertexData[i * vertexStride + 2];
    }

    centerX /= vertexCount;
    centerY /= vertexCount;
    centerZ /= vertexCount;

    // Adjust each vertex to center it around the origin
    for (int i = 0; i < vertexCount; ++i) {
        vertexData[i * vertexStride] -= centerX;
        vertexData[i * vertexStride + 1] = -(vertexData[i * vertexStride + 1] - centerY);
        vertexData[i * vertexStride + 2] -= centerZ;
    }
}

void Shape::setRenderData(const float vertexData[], int vertexDataSize, const int index[], int indexSize) {
    if (renderData.VAO > 0) {
        Renderer::destroyRenderData(renderData);
    }
    // Update the size of the index (The size will depend on the shape drawn)
    this->indexSize = indexSize;

    // Create the render data for the buffers
    renderData = Renderer::createRenderData(vertexData, vertexDataSize, index, indexSize);
}

void Shape::internalDraw() {
    glm::mat4 trs = glm::mat4(1.0f);

    trs = glm::translate(glm::mat4(1.0f), glm::vec3(transform.position.x, transform.position.y, transform.position.z));
    trs = glm::rotate(trs, glm::radians(transform.rotationQuat.x), glm::vec3(1.0, 0.0f, 0.0f));
    trs = glm::rotate(trs, glm::radians(transform.rotationQuat.y), glm::vec3(0.0f, 1.0f, 0.0f));
    trs = glm::rotate(trs, glm::radians(transform.rotationQuat.z), glm::vec3(0.0f, 0.0f, 1.0f));
    trs = glm::scale(trs, glm::vec3(transform.scale.x, transform.scale.y, 1.0f));

    Renderer::setModelMatrix(trs);
    Renderer::drawElements(renderData, indexSize);
}
