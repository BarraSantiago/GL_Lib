#pragma once

#include "shape.h"

#include <iostream>
#include <vector>

namespace gllib {
	struct UVCoords {
		float u, v;
	};

	struct Frame {
		unsigned int textureID;
		UVCoords uvCoords[4];
	};

	class DLLExport Sprite : public Shape {
	private:
		Color color;
		bool mirrorX;
		bool mirrorY;

	protected:
		std::vector<std::vector<Frame>> textures;
		int currentFrame;
		int frameCount;
		int currentAnimation = 0;
		
		void updateRenderData();
		int getCurrentFrame();
		int getFrameCount();
	public:
		Sprite(Vector3 translation, Vector3 rotation, Vector3 scale, Color color);
		Sprite(Transform transform, Color color);
		Sprite(Sprite const& other);
		virtual ~Sprite() override;

		Color getColor();

		void setColor(Color color);
		void setCurrentAnimation(int animation);
		void setCurrentFrame(unsigned int frame);
		void setCurrentFrameNext();
		void setMirroredX(bool mirrored);
		void setMirroredY(bool mirrored);

		void addTexture(unsigned int textureID);
		void addTexture(std::string path, bool transparent);

		void addFrame(unsigned int textureID, int offsetX, int offsetY, int width, int height);
		void addRawFrame(unsigned int textureID, float offsetX, float offsetY, float width, float height);
		int getCurrentAnimationFrameCount() const;

		virtual void draw() override;
	};
}
