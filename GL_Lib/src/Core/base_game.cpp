#include "base_game.h"

#include <iostream>

#include "Input.h"

using namespace gllib;
using namespace std;

// Constr/Destr

BaseGame::BaseGame() {
	window = new Window(640, 480, "Loading...");

	// Confirm that the window has been properly initialized
	if (!window->getIsInitialized()) {
		cout << "Failed to create window!\n";
		glfwTerminate();
		return;
	}
	// Make the window's context current
	window->makeContextCurrent();
	// Initialize the GLAD library
	gllib::LibCore::initGlad();
	
	input = new Input(window->getReference());
}

BaseGame::~BaseGame() {
	delete input;
	delete window;
}

// Private

bool BaseGame::initInternal() {
	// Check if the PC has a working OpenGL driver
	cout << glGetString(GL_VERSION) << "\n";

	// Load vertex and fragment shaders from files
	const char* vertexSource1 = gllib::Shader::loadShader("solidColorV.glsl");
	const char* fragmentSource1 = gllib::Shader::loadShader("solidColorF.glsl");
	const char* vertexSource2 = gllib::Shader::loadShader("textureV.glsl");
	const char* fragmentSource2 = gllib::Shader::loadShader("textureF.glsl");
	// Create shader program
	shaderProgramSolidColor = gllib::Shader::createShader(vertexSource1, fragmentSource1);
	shaderProgramTexture = gllib::Shader::createShader(vertexSource2, fragmentSource2);
	// Set current shader program
	Shader::useShaderProgram(shaderProgramSolidColor);

	init();
	updateInternal();
	Shader::destroyShader(shaderProgramSolidColor);
	Shader::destroyShader(shaderProgramTexture);
	return true;
}

void BaseGame::updateInternal() {
	// Loop until the user closes the window
	while (!window->getShouldClose()) {

		update();

		// Swap front and back buffers
		window->swapBuffers();
		// Poll for and process events
		LibCore::pollEvents();
	}
}

void BaseGame::uninitInternal() {
	uninit();
}

// Public

void BaseGame::start() {
	initInternal();
	uninitInternal();
}