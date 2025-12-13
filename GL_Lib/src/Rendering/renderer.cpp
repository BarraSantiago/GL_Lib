#include "renderer.h"

#include <iostream>

using namespace gllib;
using namespace std;

glm::mat4 Renderer::projMatrix = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);
glm::mat4 Renderer::viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
glm::mat4 Renderer::modelMatrix = glm::mat4(1.0f);

void Renderer::setUpVertexAttributes() {
    // position attribute
    // Pointer id 0, length is 3 floats (xyz), each line is 9 floats long in total (xyz,rgba,uv), value begins at position 0 on this line. 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), static_cast<void *>(nullptr));
    glEnableVertexAttribArray(0);
    // color attribute
    // Pointer id 1, length is 4 floats (rgba), value begins at position 3 on this line (after xyz). 
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // uv attribute
    // Pointer id 2, length is 2 floats (uv), value begins at position 7 on this line (after xyzrgba).
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), reinterpret_cast<void *>(7 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

void Renderer::setUpMVP() {
    // TRS
    // the mpv matrix is calculated multiplying p*v*m
    glm::mat4 mvp = projMatrix * viewMatrix * modelMatrix;
    GLint prog = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &prog);
    int mvpLocation = glGetUniformLocation(prog, "u_MVP");
    glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));
}

unsigned int Renderer::createVertexArrayObject() {
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    return VAO;
}

unsigned int Renderer::createVertexBufferObject(const float vertexData[], GLsizei bufferSize) {
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, bufferSize, vertexData, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return VBO;
}

unsigned int Renderer::createElementBufferObject(const int index[], GLsizei bufferSize) {
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferSize, index, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return EBO;
}

RenderData Renderer::createRenderData(const float vertexData[], GLsizei vertexDataSize, const int index[], GLsizei indexSize) {
    RenderData rData;

    // VAO will store the attribute pointers of our buffer.
    rData.VAO = createVertexArrayObject();
    glBindVertexArray(rData.VAO); // We need to bind VAO before setting up the attributes.

    // VBO will store the data of the vertices such as; position, color, alpha, texture coords, etc.
    rData.VBO = createVertexBufferObject(vertexData, vertexDataSize * sizeof(float));
    // EBO will store the indices, this will define the order in which we're drawing.
    rData.EBO = createElementBufferObject(index, indexSize * sizeof(int));

    // Before we set the attributes we need to have the VAO binded because that's where the pointers to these attributes will be saved.
    // We also have to bind VBO because we need to tell OpenGL which buffer we'll be using.
    glBindBuffer(GL_ARRAY_BUFFER, rData.VBO);
    setUpVertexAttributes();
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // We need to specify the color blending to use the alpha channel.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //cout << "Render data created! VAO: " << rData.VAO << ", VBO: " << rData.VBO << ", EBO: " << rData.EBO << ".\n";

    // Unbinding after finishing simply for the sake of better binding understanding.
    glBindVertexArray(0);
    return rData;
}

void Renderer::destroyRenderData(RenderData rData) {
    glDeleteBuffers(1, &rData.EBO);
    glDeleteBuffers(1, &rData.VBO);
    glDeleteVertexArrays(1, &rData.VAO);

    //printf("Render data destroyed.\n");
}

void Renderer::drawElements(RenderData rData, GLsizei indexSize) {
    setUpMVP();
    glBindVertexArray(rData.VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rData.EBO);

    glDrawElements(GL_TRIANGLES, indexSize, GL_UNSIGNED_INT, 0);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Renderer::drawTexture(RenderData rData, GLsizei indexSize, unsigned int textureID)
{
    bindTexture(textureID);
    drawElements(rData, indexSize);
}

void Renderer::bindTexture(unsigned int textureID) {
    glBindTexture(GL_TEXTURE_2D, textureID);
}

void Renderer::getTextureSize(unsigned int textureID, int* width, int* height) {
    bindTexture(textureID);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, height);
    bindTexture(0);
}

void Renderer::setModelMatrix(glm::mat4 newModelMatrix) {
    modelMatrix = newModelMatrix;
}

void Renderer::setOrthoProjectionMatrix(float width, float height) {
    projMatrix = glm::ortho(0.0f, width, height, 0.0f, -1.0f, 1.0f);
}

void Renderer::clear() {
    glClear(GL_COLOR_BUFFER_BIT);
}