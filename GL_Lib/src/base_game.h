#include "deps.h"

#include "core.h"
#include "Input.h"
#include "window.h"
#include "renderer.h"
#include "lib_time.h"
#include "loader.h"

#include "Entities/triangle.h"
#include "Entities/rectangle.h"
#include "Entities/sprite.h"
#include "Entities/animation.h"
#include "shader.h"
#include "Camera.h"
#include "CameraController.h"
#include "Entities/Cube.h"
#include "Light/AmbientLight.h"
#include "Light/PointLight.h"
#include "Material.h"
#include "transform.h"
#include "Importer/ModelImporter.h"

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