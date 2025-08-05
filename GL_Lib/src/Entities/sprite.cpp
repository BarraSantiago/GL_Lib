#include "sprite.h"
#include "../Importer/loader.h"

using namespace gllib;
using namespace std;

Sprite::Sprite(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale, Color color) :
    Shape(translation, rotation, scale) {
    this->color = color;
    mirrorX = false;
    mirrorY = false;
    currentFrame = 0;
    frameCount = 0;
    updateRenderData();
    cout << "Created sprite.\n";
}

Sprite::Sprite(Transform transform, Color color) :
    Shape(transform) {
    this->color = color;
    mirrorX = false;
    mirrorY = false;
    currentFrame = 0;
    frameCount = 0;
    updateRenderData();
    cout << "Created sprite.\n";
}

Sprite::Sprite(Sprite const& other) :
    Shape(other.transform),
    textures(other.textures),
    color(other.color),
    currentFrame(other.currentFrame),
    frameCount(other.frameCount),
    mirrorX(other.mirrorX),
    mirrorY(other.mirrorY)
{
    updateRenderData();
    cout << "Created sprite.\n";
}

Sprite::~Sprite() {
    cout << "Destroyed sprite.\n";
}

// Private

void Sprite::updateRenderData() {
    float uMin = 0.0f;
    float vMin = 0.0f;
    float uMax = 1.0f;
    float vMax = 1.0f;

    if (!textures.empty()) {
        uMin = textures[currentFrame].uvCoords[2].u;
        vMin = textures[currentFrame].uvCoords[2].v;
        uMax = textures[currentFrame].uvCoords[0].u;
        vMax = textures[currentFrame].uvCoords[0].v;
    }

    // Check for mirroring 
    if (mirrorX) { 
        std::swap(uMin, uMax); 
    } 
    if (mirrorY) { 
        std::swap(vMin, vMax); 
    }

    // Values for the vertices
    float rectangleVertexData[] = {
         1.0f, 1.0f, 0.0f, /* xyz */ color.r, color.g, color.b, color.a, /* rgba */ uMax, vMin, /* uv */
         1.0f, 0.0f, 0.0f, /* xyz */ color.r, color.g, color.b, color.a, /* rgba */ uMax, vMax, /* uv */
         0.0f, 0.0f, 0.0f, /* xyz */ color.r, color.g, color.b, color.a, /* rgba */ uMin, vMax, /* uv */
         0.0f, 1.0f, 0.0f, /* xyz */ color.r, color.g, color.b, color.a, /* rgba */ uMin, vMin, /* uv */
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

// protected

int Sprite::getCurrentFrame() {
    return currentFrame;
}

int Sprite::getFrameCount() {
    return frameCount;
}

// public

Color Sprite::getColor() {
    return color;
}

void Sprite::setColor(Color color) {
    this->color = color;
    updateRenderData();
}

void Sprite::setCurrentFrame(unsigned int index) {
    if (currentFrame == index) return;
    if (index > textures.size() - 1) return;
    currentFrame = index;
    updateRenderData();
}

void Sprite::setCurrentFrameNext() {
    currentFrame++;
    if (currentFrame == textures.size()) currentFrame = 0;
    updateRenderData();
}

void Sprite::setMirroredX(bool mirrored) {
    mirrorX = mirrored;
    updateRenderData();
}

void Sprite::setMirroredY(bool mirrored) {
    mirrorY = mirrored;
    updateRenderData();
}

void Sprite::addTexture(unsigned int textureID) {
    Frame tex;
    tex.textureID = textureID;
    if (tex.textureID == 0) return;
    tex.uvCoords[0] = { 1.0f, 1.0f };
    tex.uvCoords[1] = { 1.0f, 0.0f };
    tex.uvCoords[2] = { 0.0f, 0.0f };
    tex.uvCoords[3] = { 0.0f, 1.0f };
    textures.push_back(tex);
    currentFrame = textures.size() - 1;
    frameCount = currentFrame;
    updateRenderData();
}

void Sprite::addTexture(string path, bool transparent) {
    unsigned int texID = Loader::loadTexture(path, transparent);
    addTexture(texID);
}

void Sprite::addFrame(unsigned int textureID, int offsetX, int offsetY, int width, int height) {
    Frame tex;
    tex.textureID = textureID;
    if (tex.textureID == 0) return;
    int textureWidth = 0, textureHeight = 0;
    Renderer::getTextureSize(tex.textureID, &textureWidth, &textureHeight);

    float uMin = static_cast<float>(offsetX) / textureWidth;
    float vMin = static_cast<float>(offsetY) / textureHeight;
    float uMax = static_cast<float>(offsetX + width) / textureWidth;
    float vMax = static_cast<float>(offsetY + height) / textureHeight;

    tex.uvCoords[0] = { uMax, vMax };
    tex.uvCoords[1] = { uMax, vMin };
    tex.uvCoords[2] = { uMin, vMin };
    tex.uvCoords[3] = { uMin, vMax };

    textures.push_back(tex);
    currentFrame = textures.size() - 1;
    frameCount = currentFrame;
    updateRenderData();
}

void Sprite::draw() {
    if (!textures.empty()) {
        Renderer::bindTexture(textures[currentFrame].textureID);
    }
    internalDraw();
}