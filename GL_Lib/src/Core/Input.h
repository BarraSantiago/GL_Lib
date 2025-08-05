#pragma once
#include "deps.h"
#include <glfw-3.4.bin.WIN32/include/GLFW/glfw3.h>


enum Keys
{
    Key_A = 65,
    Key_B,
    Key_C,
    Key_D,
    Key_E,
    Key_F,
    Key_G,
    Key_H,
    Key_I,
    Key_J,
    Key_K,
    Key_L,
    Key_M,
    Key_N,
    Key_O,
    Key_P,
    Key_Q,
    Key_R,
    Key_S,
    Key_T,
    Key_U,
    Key_V,
    Key_W,
    Key_X,
    Key_Y,
    Key_Z
};

class DLLExport Input {
public:
    // Constructor
    Input(GLFWwindow* window);

    // Key input functions
    static bool getKeyPressed(int key);
    static bool getKeyReleased(int key);

    // Mouse input functions
    static bool getMouseButtonDown(int button);
    static bool getMouseButtonUp(int button);
    
    static float getMouseX();
    static float getMouseY();
    static bool isAnyKeyPressed();
private:
    static GLFWwindow* window;
    bool keys[GLFW_KEY_LAST + 1] = { false };
    bool mouseButtons[GLFW_MOUSE_BUTTON_LAST + 1] = { false };
    
};
