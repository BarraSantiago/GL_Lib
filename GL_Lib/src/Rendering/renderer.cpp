#include "renderer.h"

#include <iostream>
#include <string>
#include <vector>

#include "Importer/Mesh.h"
#include "Light/AmbientLight.h"
#include "Light/PointLight.h"

using namespace gllib;
using namespace std;

glm::mat4 Renderer::projMatrix = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
glm::mat4 Renderer::viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
glm::mat4 Renderer::modelMatrix = glm::mat4(1.0f);

void Renderer::setUpVertexAttributes()
{
    // position attribute
    // Pointer id 0, length is 3 floats (xyz), each line is 9 floats long in total (xyz,rgba,uv), value begins at position 0 on this line. 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(0);
    // color attribute
    // Pointer id 1, length is 4 floats (rgba), value begins at position 3 on this line (after xyz). 
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // uv attribute
    // Pointer id 2, length is 2 floats (uv), value begins at position 7 on this line (after xyzrgba).
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), reinterpret_cast<void*>(7 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

void Renderer::setUpMVP()
{
    // TRS
    // the mpv matrix is calculated multiplying p*v*m
    glm::mat4 mvp = projMatrix * viewMatrix * modelMatrix;
    GLint prog = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &prog);
    int mvpLocation = glGetUniformLocation(prog, "u_MVP");
    glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));
}

unsigned int Renderer::createVertexArrayObject()
{
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    return VAO;
}

unsigned int Renderer::createVertexBufferObject(const float vertexData[], GLsizei bufferSize)
{
    unsigned int VBO;
    glEnable(GL_DEPTH_TEST);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, bufferSize, vertexData, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return VBO;
}

unsigned int Renderer::createElementBufferObject(const int index[], GLsizei bufferSize)
{
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferSize, index, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return EBO;
}

RenderData Renderer::createRenderData(const float vertexData[], GLsizei vertexDataSize, const int index[],
                                      GLsizei indexSize)
{
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

    // Unbinding after finishing simply for the sake of better binding understanding.
    glBindVertexArray(0);
    return rData;
}

void Renderer::destroyRenderData(RenderData rData)
{
    glDeleteBuffers(1, &rData.EBO);
    glDeleteBuffers(1, &rData.VBO);
    glDeleteVertexArrays(1, &rData.VAO);
}

void Renderer::drawElements(RenderData rData, GLsizei indexSize)
{
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

void Renderer::drawEntity3D(unsigned& VAO, unsigned indexQty, Material& material, glm::mat4 trans)
{
    glUseProgram(shader3DProgram);

    // Set transformation matrices
    glUniformMatrix4fv(glGetUniformLocation(shader3DProgram, "model"), 1, GL_FALSE, glm::value_ptr(trans));
    glUniformMatrix4fv(glGetUniformLocation(shader3DProgram, "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader3DProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projMatrix));

    // Set material properties
    glUniform3fv(glGetUniformLocation(shader3DProgram, "material.diffuse"), 1, glm::value_ptr(material.diffuse));
    glUniform3fv(glGetUniformLocation(shader3DProgram, "material.specular"), 1, glm::value_ptr(material.specular));
    glUniform1f(glGetUniformLocation(shader3DProgram, "material.shininess"), material.shininess);

    // Explicitly set hasTexture to false for entities without textures
    glUniform1i(glGetUniformLocation(shader3DProgram, "material.hasTexture"), 0);

    // Apply lights from your Light system
    int lightIndex = 0;
    for (Light* light : Light::lights)
    {
        light->apply(shader3DProgram);
        lightIndex++;
        if (lightIndex >= 8) break;
    }

    // Set view position (camera position)
    glUniform3f(glGetUniformLocation(shader3DProgram, "viewPos"), 0.0f, 0.0f, 3.0f);

    // Draw the mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexQty, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glUseProgram(0);
}

void Renderer::drawModel3D(unsigned& VAO, unsigned indexQty, glm::mat4 trans, std::vector<Texture>& textures)
{
    glUseProgram(shader3DProgram);
    glUniformMatrix4fv(glGetUniformLocation(shader3DProgram, "model"), 1, GL_FALSE, glm::value_ptr(trans));
    glUniformMatrix4fv(glGetUniformLocation(shader3DProgram, "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader3DProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projMatrix));

    // Set material properties
    glUniform1f(glGetUniformLocation(shader3DProgram, "material.shininess"), 32.0f);

    // Bind textures
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    for (unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        std::string number;
        std::string name = textures[i].type;
        if (name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if (name == "texture_specular")
            number = std::to_string(specularNr++);

        GLint u_Material = glGetUniformLocation(shader3DProgram, ("material." + name + number).c_str());
        glUniform1i(u_Material, i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    // Set hasTexture flag
    bool hasTextures = !textures.empty();
    glUniform1i(glGetUniformLocation(shader3DProgram, "material.hasTexture"), hasTextures ? 1 : 0);

    // Apply lights from your Light system
    int lightIndex = 0;
    for (Light* light : Light::lights)
    {
        light->apply(shader3DProgram);
        lightIndex++;
        if (lightIndex >= 8) break;
    }

    // Set view position (camera position)
    glUniform3f(glGetUniformLocation(shader3DProgram, "viewPos"), 0.0f, 0.0f, 3.0f);

    // Draw the mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexQty, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Unbind textures
    for (unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glActiveTexture(GL_TEXTURE0);
    glUseProgram(0);
}

void Renderer::bindTexture(unsigned int textureID)
{
    glBindTexture(GL_TEXTURE_2D, textureID);
}

void Renderer::getTextureSize(unsigned int textureID, int* width, int* height)
{
    bindTexture(textureID);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, height);
    bindTexture(0);
}

void Renderer::setModelMatrix(glm::mat4 newModelMatrix)
{
    modelMatrix = newModelMatrix;
}

void Renderer::setOrthoProjectionMatrix(float width, float height)
{
    projMatrix = glm::ortho(0.0f, width, height, 0.0f, -1.0f, 1.0f);
}

void Renderer::setPerspectiveProjectionMatrix(float fov, float aspectRatio, float nearPlane, float farPlane)
{
    projMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
}

glm::mat4 Renderer::getViewMatrix()
{
    return viewMatrix;
}

void Renderer::setViewMatrix(glm::mat4 newViewMatrix)
{
    viewMatrix = newViewMatrix;
}

void Renderer::clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::genVertexBuffer(unsigned int& VBO, unsigned int& VAO, float vertices[], unsigned int id,
                               unsigned int qty)
{
    glGenVertexArrays(id, &VAO);
    glGenBuffers(id, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * qty * 8, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), static_cast<void*>(0));
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

void Renderer::genIndexBuffer(unsigned int& IBO, unsigned int indices[], unsigned int id, unsigned int qty)
{
    glGenBuffers(id, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * qty, indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Renderer::deleteBuffers(unsigned int& VBO, unsigned int& IBO, unsigned int& EBO, unsigned int id)
{
    glDeleteVertexArrays(id, &VBO);
    glDeleteBuffers(id, &IBO);
    glDeleteBuffers(id, &EBO);
}

void Renderer::glClearError()
{
    while (glGetError());
}

bool Renderer::glLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (" << error << "): "
            << function << " " << file << ": " << line << std::endl;
        return false;
    }

    return true;
}
