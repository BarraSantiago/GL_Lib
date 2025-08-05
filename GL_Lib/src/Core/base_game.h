#include "deps.h"

#include "core.h"
#include "Input.h"
#include "lib_time.h"

#include "Entities/triangle.h"
#include "Entities/rectangle.h"
#include "Entities/sprite.h"
#include "Entities/animation.h"
#include "Entities/Cube.h"
#include "Importer/ModelLoader.h"
#include "Importer/Model.h"
#include "Importer/Mesh.h"
#include "Rendering/Camera/Camera.h"
#include "Rendering/Camera/CameraController.h"
#include "Window/window.h"
#include <Rendering/Light/AmbientLight.h>
#include <Rendering/Light/PointLight.h>
#include "Math/collisionManager.h"
#include "Rendering/Light/SpotLight.h"

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
		ModelLoader* importer;

		unsigned int shaderProgramSolidColor;
		unsigned int shaderProgramTexture;
		unsigned int shaderProgramLighting;

		virtual void init() {}
		virtual void update() {}
		virtual void uninit() {}

	public:
		BaseGame();
		virtual ~BaseGame();

		Camera getCamera() { return *camera; }
		CameraController* getCameraController() { return cameraController; }
		
		void start();
	};

}
