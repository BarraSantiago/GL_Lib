// General dependencies
#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#ifdef _WIN32
#define DLLExport __declspec(dllexport)
#else
#define DLLExport __attribute__((visibility("default")))
#endif
