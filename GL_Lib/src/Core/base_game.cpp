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

	if (!Shader::initDefaultShaders()) {
		return false;
	}

	// Set current shader program
	Shader::useShaderProgram(Shader::shapeShaderProgram);

	init();
	
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
	Shader::destroyDefaultShaders();
	uninit();
}

// Public

void BaseGame::start() {
	if(initInternal())
	{
		updateInternal();
	}
	uninitInternal();
}
