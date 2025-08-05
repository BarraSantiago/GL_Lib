#pragma once
#include "deps.h"

namespace gllib {

	class DLLExport LibCore {
	public:
		LibCore();
		~LibCore();

		/// <summary>
		/// Can only be called after initializing a window and setting up the glfw context
		/// </summary>
		static bool initGlad();

		/// <summary>
		/// Must be called on every frame to poll glfw and other library utils
		/// </summary>
		static void pollEvents();
	};

}