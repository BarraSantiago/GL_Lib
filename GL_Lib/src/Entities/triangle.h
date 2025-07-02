#pragma once

#include "shape.h"

namespace gllib {

    class DLLExport Triangle: public Shape {
    private:
        Color color;

        void updateRenderData(Color color);
    public:
        Triangle(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale, Color color);
        Triangle(Transform transform, Color color);
        virtual ~Triangle() override;

        Color getColor();

        void setColor(Color color);

        virtual void draw() override;
    };
}
