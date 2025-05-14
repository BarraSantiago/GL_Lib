#pragma once
#include "glm.hpp"
#include "Light.h"
#include "transform.h"
#include "Shader.h"

namespace gllib
{
    class DLLExport AmbientLight : public Light
    {
    private:
        float intensity;

    public:
        AmbientLight(const Color& color = {1.0f, 1.0f, 1.0f, 1.0f}, float intensity = 0.1f)
            : Light(color), intensity(intensity)
        {
        }

        void setIntensity(float newIntensity) { intensity = newIntensity; }
        float getIntensity() const { return intensity; }

        void apply(unsigned int shaderProgram) const override
        {
            Shader::setVec3(shaderProgram, "ambientStrength",
                            color.r * intensity,
                            color.g * intensity,
                            color.b * intensity);
        }
    };
}
