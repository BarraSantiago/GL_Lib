#pragma once
#include "Core/deps.h"

#include <iostream>

namespace gllib {
	class DLLExport Loader {
	public:
		static bool fileExists(std::string filePath);
		static unsigned int loadTextureAdvanced(std::string filePath, GLint wrapping, GLint filtering, bool transparent);
		static unsigned int loadTexture(std::string filePath, bool transparent);
		static const char* loadTextFile(std::string filePath);
		static void unloadTexture(unsigned int id);
	};
}