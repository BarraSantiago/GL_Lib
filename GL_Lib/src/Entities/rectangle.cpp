#include "rectangle.h"

#include <iostream>

using namespace gllib;
using namespace std;

Rectangle::Rectangle(Vector3 translation, Vector3 rotation, Vector3 scale, Color color) :
    Shape(translation, rotation, scale) {
    this->color = color;
    updateRenderData(this->color);
    cout << "Created rectangle.\n";
}

Rectangle::Rectangle(Transform transform, Color color) :
    Shape(transform) {
    this->color = color;
    updateRenderData(this->color);
    cout << "Created rectangle.\n";
}

Rectangle::~Rectangle() {
    cout << "Destroyed rectangle.\n";
}

void Rectangle::updateRenderData(Color color) {
    // Values for the vertices
    float rectangleVertexData[] = {
         1.0f, 1.0f, 0.0f, /* xyz */ color.r, color.g, color.b, color.a, /* rgba */ 1.0f, 1.0f, /* uv */
         1.0f, 0.0f, 0.0f, /* xyz */ color.r, color.g, color.b, color.a, /* rgba */ 1.0f, 0.0f, /* uv */
         0.0f, 0.0f, 0.0f, /* xyz */ color.r, color.g, color.b, color.a, /* rgba */ 0.0f, 0.0f, /* uv */
         0.0f, 1.0f, 0.0f, /* xyz */ color.r, color.g, color.b, color.a, /* rgba */ 0.0f, 1.0f, /* uv */
    };
    // The order in which the vertices are drawn
    const int rectangleIndex[] = {
        0, 1, 3, // First triangle
        1, 2, 3  // Second traingle
    };

    alignVertex(rectangleVertexData, 4, 9);

    // Aquire the size of each buffer
    int vertexDataSize = sizeof(rectangleVertexData) / sizeof(rectangleVertexData[0]);
    int indexSize = sizeof(rectangleIndex) / sizeof(rectangleIndex[0]);

    // Initialize the render data on shape
    setRenderData(rectangleVertexData, vertexDataSize, rectangleIndex, indexSize);
}

Color Rectangle::getColor() {
    return color;
}

void Rectangle::setColor(Color color) {
    this->color = color;
    updateRenderData(this->color);
}

void Rectangle::draw() {

    // Shape has an internal draw function
    internalDraw();
}