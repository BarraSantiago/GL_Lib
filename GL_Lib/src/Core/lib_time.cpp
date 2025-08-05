#include "lib_time.h"

#include "deps.h"

using namespace gllib;

double LibTime::previousFrameTime = 0.00;
double LibTime::currentFrameTime = 0.00;

double LibTime::fpsUpdateTime = 0.00;
int LibTime::frameCount = 0;
double LibTime::fps = 0.00;

int LibTime::getFPS() {
	return (int)(fps);
}

double LibTime::getDeltaTime() {
	return currentFrameTime - previousFrameTime;
}

double LibTime::getElapsedTime() {
	return glfwGetTime();
}

void LibTime::updateFrameTime() {
	previousFrameTime = currentFrameTime;
	currentFrameTime = getElapsedTime();

	frameCount++;
	if (currentFrameTime - fpsUpdateTime >= 1.0) {
		fps = (double)(frameCount);
		frameCount = 0;
		fpsUpdateTime = currentFrameTime;
	}
}