#pragma once
#include "Core/deps.h"
#include "Rendering/Light/Material.h"
#include "Entities/Entity2.h"
#include "Importer/Mesh.h"

#ifdef _WIN32 // Directory is different in linux
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#else
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#endif // _WIN32

#pragma region MACROS
#define ASSERT(x) if (!(x)) __debugbreak()
#define glCall(x) glClearError();\
x;\
ASSERT(glLogCall(#x, __FILE__, __LINE__))
#pragma endregion

namespace gllib
{
    /// <summary>
    /// This struct contains the 3 basic buffers needed to draw using glDrawElements
    /// </summary>
    struct DLLExport RenderData
    {
        unsigned int VAO; // Vertex Array Object
        unsigned int VBO; // Vertex Buffer Object
        unsigned int EBO; // Element Buffer Object
    };

    /// <summary>
    /// Fully static class
    /// </summary>
    class DLLExport Renderer
    {
    private:
        static glm::mat4 projMatrix;
        static glm::mat4 modelMatrix;
        static glm::mat4 viewMatrix;
        
        static void glClearError();
        static bool glLogCall(const char* function, const char* file, int line);

    public:
        inline static glm::uint shader3DProgram = 0;
        static void setUpVertexAttributes();
        static void setUpMVP();

        static unsigned int createVertexArrayObject();
        static unsigned int createVertexBufferObject(const float vertexData[], GLsizei bufferSize);
        static unsigned int createElementBufferObject(const int index[], GLsizei bufferSize);

        static RenderData createRenderData(const float vertexData[], GLsizei vertexDataSize, const int index[],
                                           GLsizei indexSize);
        static void destroyRenderData(RenderData rData);

        static void drawElements(RenderData rData, GLsizei indexSize);
        static void drawTexture(RenderData rData, GLsizei indexSize, unsigned int textureID);
        static void drawEntity3D(unsigned& VAO, unsigned indexQty, Material& material, glm::mat4 trans);
        static void drawModel3D(unsigned& VAO, unsigned indexQty, glm::mat4 trans, std::vector<Texture>& textures);

        static void bindTexture(unsigned int textureID);
        static void getTextureSize(unsigned int textureID, int* width, int* height);

        static void setModelMatrix(glm::mat4 newModelMatrix);
        static void setOrthoProjectionMatrix(float width, float height);
        static void setPerspectiveProjectionMatrix(float fov, float aspectRatio, float nearPlane, float farPlane);

        static glm::mat4 getViewMatrix();
        static void setViewMatrix(glm::mat4 newViewMatrix);

        static void clear();
        static void genVertexBuffer(unsigned int& VBO, unsigned int& VAO, float vertices[], unsigned int id,
                                    unsigned int qty);
        static void genIndexBuffer(unsigned int& IBO, unsigned int indices[], unsigned int id, unsigned int qty);
        static void deleteBuffers(unsigned int& VBO, unsigned int& IBO, unsigned int& EBO, unsigned int id);
    };
}
