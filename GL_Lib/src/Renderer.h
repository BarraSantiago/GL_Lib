#pragma once
#include <vector>

#include "deps.h"

#ifdef _WIN32 // Directory is different in linux
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "Importer/Mesh.h"
#else
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#endif // _WIN32

namespace gllib {

	/// <summary>
	/// This struct contains the 3 basic buffers needed to draw using glDrawElements
	/// </summary>
	struct DLLExport RenderData {
		unsigned int VAO; // Vertex Array Object
		unsigned int VBO; // Vertex Buffer Object
		unsigned int EBO; // Element Buffer Object
	};

	/// <summary>
	/// Fully static class
	/// </summary>
	class DLLExport Renderer {
	private:
		static glm::mat4 projMatrix;
		static glm::mat4 modelMatrix;
		static glm::mat4 viewMatrix;

	public:
		static void setUpVertexAttributes();
		static void setUpMVP();

		static unsigned int createVertexArrayObject();
		static unsigned int createVertexBufferObject(const float vertexData[], GLsizei bufferSize);
		static unsigned int createElementBufferObject(const int index[], GLsizei bufferSize);

		static RenderData createRenderData(const float vertexData[], GLsizei vertexDataSize, const int index[], GLsizei indexSize);
		static void destroyRenderData(RenderData rData);

		static void drawElements(RenderData rData, GLsizei indexSize);
		static void drawTexture(RenderData rData, GLsizei indexSize, unsigned int textureID);

		static void bindTexture(unsigned int textureID);
		static void getTextureSize(unsigned int textureID, int* width, int* height);

		static void setModelMatrix(glm::mat4 newModelMatrix);
		static void setOrthoProjectionMatrix(float width, float height);
		static void setPerspectiveProjectionMatrix(float fov, float aspectRatio, float nearPlane, float farPlane);
		static void drawMesh(const std::vector<unsigned int>& indices, const std::vector<Texture>& textures, unsigned int VAO, const glm::vec3& color);
		static glm::mat4 getViewMatrix();
		static void setViewMatrix(glm::mat4 newViewMatrix);
		
		static void clear();
	};

}
