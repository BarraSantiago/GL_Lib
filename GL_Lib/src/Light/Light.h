#pragma once
#include <list>

#include "glm.hpp"
#include "../transform.h"
#include "../Shader.h"

namespace gllib
{
    class DLLExport Light
    {
    protected:
        Color color;

    public:
        inline  static std::list<Light*> lights;

        Light(const Color& color = {1.0f, 1.0f, 1.0f, 1.0f}): color(color)
        {
            lights.push_back(this);
        }

        virtual ~Light();

        void setColor(const Color& newColor) { color = newColor; }
        Color getColor() const { return color; }

        virtual void apply(unsigned int shaderProgram) const = 0;
    };
}
