#include "base_game.h"

#include <iostream>

#include "Input.h"
#include "Rendering/renderer.h"
#include "Rendering/Shader.h"

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
    LibCore::initGlad();

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
    delete importer;
}

// Private

bool BaseGame::initInternal()
{
    // Check if the PC has a working OpenGL driver
    cout << glGetString(GL_VERSION) << "\n";

    // Load vertex and fragment shaders from files
    const char* vertexSource1 = Shader::loadShader("solidColorV.glsl");
    const char* fragmentSource1 = Shader::loadShader("solidColorF.glsl");
    const char* vertexSource2 = Shader::loadShader("textureV.glsl");
    const char* fragmentSource2 = Shader::loadShader("textureF.glsl");
    const char* vertexLightingSource = Shader::loadShader("lightingV.glsl");
    const char* fragmentLightingSource = Shader::loadShader("lightingF.glsl");


    // Create shader program
    shaderProgramSolidColor = Shader::createShader(vertexSource1, fragmentSource1);
    shaderProgramTexture = Shader::createShader(vertexSource2, fragmentSource2);
    shaderProgramLighting = Shader::createShader(vertexLightingSource, fragmentLightingSource);
    Renderer::shader3DProgram = shaderProgramLighting;
    // Set current shader program
    Shader::setShaderProgram(shaderProgramSolidColor);

    // Add point light parameters
    Shader::setVec3(shaderProgramLighting, "lightColor", 1.0f, 1.0f, 1.0f);
    Shader::setVec3(shaderProgramLighting, "lightPos", 5.0f, 5.0f, 5.0f);

    // Set attenuation values
    Shader::setFloat(shaderProgramLighting, "light.constant", 1.0f);
    Shader::setFloat(shaderProgramLighting, "light.linear", 0.09f);
    Shader::setFloat(shaderProgramLighting, "light.quadratic", 0.032f);
    importer = new ModelLoader();
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
