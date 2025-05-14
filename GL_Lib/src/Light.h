#pragma once
#include "glm.hpp"
#include "transform.h"
#include "Shader.h"

namespace gllib
{
    class DLLExport Light
    {
    protected:
        Color color;

    public:
        Light(const Color& color = {1.0f, 1.0f, 1.0f, 1.0f}): color(color)
        {
        }

        virtual ~Light() = default;

        void setColor(const Color& newColor) { color = newColor; }
        Color getColor() const { return color; }

        virtual void apply(unsigned int shaderProgram) const = 0;
    };
}
