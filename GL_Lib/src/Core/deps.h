// General dependencies
#pragma once

#include "../glad/include/glad/glad.h"
#include "../../../lib/glfw-3.4.bin.WIN64/include/GLFW/glfw3.h"

#ifdef _WIN32
#define DLLExport __declspec(dllexport)
#else
#define DLLExport __attribute__((visibility("default")))
#endif
