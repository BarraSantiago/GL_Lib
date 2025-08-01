#pragma once

#include "shape.h"

namespace gllib {
	class DLLExport Rectangle : public Shape {
	private:
		Color color;

		void updateRenderData(Color color);
	public:
		Rectangle(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale, Color color);
		Rectangle(Transform transform, Color color);
		virtual ~Rectangle() override;

		Color getColor();

		void setColor(Color color);

		virtual void draw() override;
	};
}