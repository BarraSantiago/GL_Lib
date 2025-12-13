#pragma once

#include "shape.h"

namespace gllib {

    class DLLExport Triangle: public Shape {
    private:
        Color color;

        void updateRenderData(Color color);
    public:
        Triangle(Vector3 translation, Vector3 rotation, Vector3 scale, Color color);
        Triangle(Transform transform, Color color);
        virtual ~Triangle() override;

        Color getColor();

        void setColor(Color color);

        virtual void draw() override;
    };
}
