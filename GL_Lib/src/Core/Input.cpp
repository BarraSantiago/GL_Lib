#include "Input.h"

#include <unordered_map>

GLFWwindow* Input::window = nullptr;

Input::Input(GLFWwindow* window)
{
    Input::window = window;
}

bool Input::getKeyPressed(int key)
{
    return (glfwGetKey(window, key) == GLFW_PRESS);
}

bool Input::getMouseButtonDown(int button)
{
    return (glfwGetMouseButton(window, button) == GLFW_PRESS);
}

bool Input::getMouseButtonUp(int button)
{
    return (glfwGetMouseButton(window, button) == GLFW_RELEASE);
}

float Input::getMouseX() 
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    return xpos;
}

float Input::getMouseY()
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    return ypos;
}

bool Input::isAnyKeyPressed()
{
    for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; ++key)
    {
        if (glfwGetKey(window, key) == GLFW_PRESS)
        {
            return true;
        }
    }
    return false;
}

bool Input::getKeyReleased(int key)
{
    static std::unordered_map<int, bool> keyStates;

    if (glfwGetKey(window, key) == GLFW_RELEASE)
    {
        if (keyStates[key])
        {
            keyStates[key] = false;
            return true;
        }
    }
    else
    {
        keyStates[key] = true;
    }

    return false;
}
