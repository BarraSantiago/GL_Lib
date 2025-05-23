#include "deps.h"

#include "core.h"
#include "Input.h"
#include "window.h"
#include "renderer.h"
#include "lib_time.h"
#include "loader.h"

#include "triangle.h"
#include "rectangle.h"
#include "sprite.h"
#include "animation.h"
#include "shader.h"
#include "Camera.h"
#include "CameraController.h"
#include "Cube.h"
#include "AmbientLight.h"
#include "PointLight.h"
#include "Material.h"
#include "transform.h"

namespace gllib {

	class DLLExport BaseGame {
	private:
		LibCore libCore;
		
		bool initInternal();
		void updateInternal();
		void uninitInternal();

	protected:
		Window* window;
		Input* input;
		Camera* camera;
		CameraController* cameraController;
		
		unsigned int shaderProgramSolidColor;
		unsigned int shaderProgramTexture;
		unsigned int shaderProgramLighting;

		virtual void init() {}
		virtual void update() {}
		virtual void uninit() {}

	public:
		BaseGame();
		virtual ~BaseGame();
		
		Camera* getCamera() { return camera; }
		CameraController* getCameraController() { return cameraController; }
		
		void start();
	};

}