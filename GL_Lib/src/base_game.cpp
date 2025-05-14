#include "base_game.h"

#include <iostream>

#include "Camera.h"
#include "CameraController.h"
#include "Input.h"

using namespace gllib;
using namespace std;

// Constr/Destr

BaseGame::BaseGame()
{
    window = new Window(640, 480, "Loading...");

    // Confirm that the window has been properly initialized
    if (!window->getIsInitialized())
    {
        cout << "Failed to create window!\n";
        glfwTerminate();
        return;
    }
    glfwSetWindowUserPointer(window->getReference(), this);
    glfwSetInputMode(window->getReference(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Make the window's context current
    window->makeContextCurrent();
    // Initialize the GLAD library
    gllib::LibCore::initGlad();

    input = new Input(window->getReference());

    camera = new Camera();
    cameraController = new CameraController(camera, 10);

    // Set up perspective projection
    camera->setPerspective(45.0f, window->getWidth() / (float)window->getHeight(), 0.1f, 100.0f);
    camera->setPosition(glm::vec3(0.0f, 0.0f, 5.0f));
    camera->setRotation(-90.0f, 0.0f);

    // Set up mouse callback for camera control
    glfwSetCursorPosCallback(window->getReference(), [](GLFWwindow* window, double xpos, double ypos)
    {
        BaseGame* game = static_cast<BaseGame*>(glfwGetWindowUserPointer(window));
        if (game)
            game->getCameraController()->processMouseMovement(xpos, ypos);
    });
}

BaseGame::~BaseGame()
{
    delete input;
    delete window;
    delete cameraController;
    delete camera;
}

// Private

bool BaseGame::initInternal()
{
    // Check if the PC has a working OpenGL driver
    cout << glGetString(GL_VERSION) << "\n";

    // Load vertex and fragment shaders from files
    const char* vertexSource1 = gllib::Shader::loadShader("solidColorV.glsl");
    const char* fragmentSource1 = gllib::Shader::loadShader("solidColorF.glsl");
    const char* vertexSource2 = gllib::Shader::loadShader("textureV.glsl");
    const char* fragmentSource2 = gllib::Shader::loadShader("textureF.glsl");
    const char* vertexLightingSource = gllib::Shader::loadShader("lightingV.glsl");
    const char* fragmentLightingSource = gllib::Shader::loadShader("lightingF.glsl");

    // Create shader program
    shaderProgramSolidColor = gllib::Shader::createShader(vertexSource1, fragmentSource1);
    shaderProgramTexture = gllib::Shader::createShader(vertexSource2, fragmentSource2);
    shaderProgramLighting = gllib::Shader::createShader(vertexLightingSource, fragmentLightingSource);

    // Set current shader program
    Shader::setShaderProgram(shaderProgramSolidColor);
    
    // Add point light parameters
    gllib::Shader::setVec3(shaderProgramLighting, "lightColor", 1.0f, 1.0f, 1.0f);
    gllib::Shader::setVec3(shaderProgramLighting, "lightPos", 5.0f, 5.0f, 5.0f);
    
    // Set attenuation values
    gllib::Shader::setFloat(shaderProgramLighting, "light.constant", 1.0f);
    gllib::Shader::setFloat(shaderProgramLighting, "light.linear", 0.09f);
    gllib::Shader::setFloat(shaderProgramLighting, "light.quadratic", 0.032f);

    init();
    updateInternal();
    Shader::destroyShader(shaderProgramSolidColor);
    Shader::destroyShader(shaderProgramTexture);
    return true;
}

void BaseGame::updateInternal()
{
    // Loop until the user closes the window
    while (!window->getShouldClose())
    {
        cameraController->processInput();
        update();

        // Swap front and back buffers
        window->swapBuffers();
        // Poll for and process events
        LibCore::pollEvents();
    }
}

void BaseGame::uninitInternal()
{
    uninit();
}

// Public

void BaseGame::start()
{
    initInternal();
    uninitInternal();
}
