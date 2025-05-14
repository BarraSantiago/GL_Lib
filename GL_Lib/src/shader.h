#pragma once
#include <fwd.hpp>

#include "deps.h"
#include "glm.hpp"
#include <gtc/matrix_transform.hpp>
#include <iostream>

namespace gllib {
	class DLLExport Shader {
	private:
		static std::string getShaderType(unsigned int type);
		static unsigned int compileShader(unsigned int type, std::string source);

	public:
		static unsigned int currentShaderProgram;
		static unsigned int shapeShaderProgram;
		static unsigned int textureShaderProgram;
		
		static unsigned int createShader(const char* vertexShader, const char* fragmentShader);
		static void destroyShader(unsigned int program);
		static const char* loadShader(std::string filePath);
		static void setShaderProgram(unsigned int shaderProgram);
		static void setVec3(unsigned int shaderProgram, const char* name, float x, float y, float z);
		static void setMat4(unsigned int programID, const char* name, const glm::mat4& value);
		static void setFloat(unsigned int shaderProgram, const char* name, float value);
		static unsigned int getCurrentShaderProgram();
	};
};