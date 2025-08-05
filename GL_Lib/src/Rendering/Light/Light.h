#pragma once
#include <list>

#include "glm.hpp"
#include "Math/transform.h"
#include "../Shader.h"
#include "Material.h"

namespace gllib
{
    class DLLExport Light
    {
    protected:
        Color color;

    public:
        inline  static std::list<Light*> lights;

        Light(const Color& color = {1.0f, 1.0f, 1.0f, 1.0f});

        virtual ~Light();

        void setColor(const Color& newColor);
        Color getColor() const;

        virtual void apply(unsigned int shaderProgram) const = 0;
    };
}
