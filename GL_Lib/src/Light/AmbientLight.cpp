#include "AmbientLight.h"

namespace gllib
{
    AmbientLight::AmbientLight(const Color& color, float intensity) : Light(color), intensity(intensity)
    {
    }

    void AmbientLight::setIntensity(float newIntensity)
    {
        intensity = newIntensity;
    }

    float AmbientLight::getIntensity() const
    {
        return intensity;
    }

    void AmbientLight::apply(unsigned int shaderProgram) const
    {
        // Set ambient color and intensity
        Shader::setVec3(shaderProgram, "ambientStrength", color.r * intensity, color.g * intensity,
                        color.b * intensity);

        // These uniforms are not needed for ambient light in your current shader
        // but keeping them for compatibility with the point light calculation
        Shader::setVec3(shaderProgram, "lightColor", color.r, color.g, color.b);
        Shader::setVec3(shaderProgram, "lightPos", 0.0f, 0.0f, 0.0f);

        // Zero out diffuse and specular components for ambient light
        Shader::setFloat(shaderProgram, "diffuseStrength", 0.0f);
        Shader::setFloat(shaderProgram, "specularStrength", 0.0f);

        // Set attenuation values to disable them for ambient light
        Shader::setFloat(shaderProgram, "light.constant", 1.0f);
        Shader::setFloat(shaderProgram, "light.linear", 0.0f);
        Shader::setFloat(shaderProgram, "light.quadratic", 0.0f);
    }
}
