#include "window.h"

#include "Renderer.h" // To set the proj matrix

using namespace gllib;
using namespace std;

Window::Window(int width, int height, string title) {
	window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
	cout << "Window created!\n";
	//Renderer::setOrthoProjectionMatrix(static_cast<float>(width), static_cast<float>(height));
	Renderer::setPerspectiveProjectionMatrix(45.0f, static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f);
}

Window::~Window() {
	// We just close it, we will be using glfwTerminate to clean up the resources.
	glfwSetWindowShouldClose(window, true);
	cout << "Window closed.\n";
}

// Public

void Window::makeContextCurrent() {
	glfwMakeContextCurrent(window);
}

void Window::swapBuffers() {
	glfwSwapBuffers(window);
}

GLFWwindow* Window::getReference() {
	return window;
}

int Window::getWidth() {
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	return width;
}

int Window::getHeight() {
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	return height;
}

bool Window::getIsInitialized() {
	return (window);
}

bool Window::getShouldClose() {
	return glfwWindowShouldClose(window);
}

void Window::setTitle(string title) {
	glfwSetWindowTitle(window, title.c_str());
}

void Window::setVsyncEnabled(bool set) {
	glfwSwapInterval(set);
}