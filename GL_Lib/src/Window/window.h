#pragma once
#include "Core/deps.h"

#include <iostream>

namespace gllib {

	class DLLExport Window {
	private:
		GLFWwindow* window;
	public:
		/// <summary>
		/// The constructor creates a window and opens it
		/// </summary>
		Window(int width, int height, std::string title);
		/// <summary>
		/// Destructor simply closes the window the function glfwTerminate should take care of uninitializing it later
		/// </summary>
		~Window();

		/// <summary>
		/// Links the window to the current glfw context
		/// </summary>
		void makeContextCurrent();
		/// <summary>
		/// Swaps render buffers
		/// </summary>
		void swapBuffers();

		/// <summary>
		/// Returns the variable window
		/// </summary>
		GLFWwindow* getReference();
		/// <summary>
		/// Returns the width of the window in pixels
		/// </summary>
		int getWidth();
		/// <summary>
		/// Returns the height of the window in pixels
		/// </summary>
		int getHeight();
		/// <summary>
		/// Used to confirm if the window has been properly initialized
		/// </summary>
		/// <returns></returns>
		bool getIsInitialized();
		/// <summary>
		/// Returns if the user has tried to close the window manually
		/// </summary>
		bool getShouldClose();
		/// <summary>
		/// Changes the title of the window to a specific string
		/// </summary>
		void setTitle(std::string title);
		/// <summary>
		/// If enabled it syncs the fps output to the refresh rate of the monitor
		/// </summary>
		void setVsyncEnabled(bool set);
	};

}