#pragma once

#include "deps.h"

namespace gllib {

	class DLLExport LibTime {
	private:
		static double previousFrameTime;
		static double currentFrameTime;
		static double fpsUpdateTime;
		static int frameCount;
		static double fps;
	public:
		static int getFPS();
		static double getDeltaTime();
		static double getElapsedTime();
		static void updateFrameTime();
	};

}