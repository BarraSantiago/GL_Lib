#pragma once

#include "shape.h"

namespace gllib {
	class DLLExport Rectangle : public Shape {
	private:
		Color color;

		void updateRenderData(Color color);
	public:
		Rectangle(Vector3 translation, Vector3 rotation, Vector3 scale, Color color);
		Rectangle(Transform transform, Color color);
		virtual ~Rectangle() override;

		Color getColor();

		void setColor(Color color);

		virtual void draw() override;
	};
}