#include "core.h"

#include "deps.h"
#include "lib_time.h"

#include <iostream>

using namespace gllib;
using namespace std;

LibCore::LibCore() {
    if (!glfwInit()) {
        cout << "Failed to initialize GLFW!\n";
    }
    else {
        cout << "GLFW initialized successfully!\n";
    }
}

LibCore::~LibCore() {
    cout << "Terminated GLFW.\n";
    glfwTerminate();
}

bool LibCore::initGlad() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "Failed to initialize GLAD\n";
        glfwTerminate();
        return false;
    }
    else {
        cout << "Initialized GLAD successfully!\n";
    }
    return true;
}


void LibCore::pollEvents() {
    LibTime::updateFrameTime();
    glfwPollEvents();
}