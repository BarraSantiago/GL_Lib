#pragma once
#include "glm.hpp"
#include "Light.h"

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
            // Set ambient strength with color and intensity combined
            Shader::setVec3(shaderProgram, "ambientStrength", color.r * intensity, color.g * intensity,
                            color.b * intensity);

            // Set light color
            Shader::setVec3(shaderProgram, "lightColor", color.r, color.g, color.b);

            // For ambient light, set light directly at camera (follows the view)
            Shader::setVec3(shaderProgram, "lightPos", 0.0f, 0.0f, 0.0f);

            // Zero out diffuse and specular components
            Shader::setFloat(shaderProgram, "diffuseStrength", 0.0f);
            Shader::setFloat(shaderProgram, "specularStrength", 0.0f);

            // Set attenuation values to effectively disable it
            // constant=1, linear=0, quadratic=0 means no distance-based attenuation
            Shader::setFloat(shaderProgram, "light.constant", 1.0f);
            Shader::setFloat(shaderProgram, "light.linear", 0.0f);
            Shader::setFloat(shaderProgram, "light.quadratic", 0.0f);
        }
    };
}
