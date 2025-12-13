#include "deps.h"

#include "core.h"
#include "Input.h"
#include "../Window/window.h"
#include "../Rendering/renderer.h"
#include "lib_time.h"
#include "../Rendering/loader.h"

#include "../Entities/triangle.h"
#include "../Entities/rectangle.h"
#include "../Entities/sprite.h"
#include "../Entities/animation.h"
#include "../Rendering/shader.h"
#include "../TileMap/TileMap.h"

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

		unsigned int shaderProgramSolidColor;
		unsigned int shaderProgramTexture;

		virtual void init() {};
		virtual void update() {};
		virtual void uninit() {};

	public:
		BaseGame();
		virtual ~BaseGame();

		void start();
	};

}