#include "AmbientLight.h"

void gllib::AmbientLight::apply(unsigned int shaderProgram) const
{
    Shader::setVec3(shaderProgram, "ambientStrength", color.r * intensity, color.g * intensity,
                    color.b * intensity);

    Shader::setVec3(shaderProgram, "lightColor", color.r, color.g, color.b);

    Shader::setVec3(shaderProgram, "lightPos", 0.0f, 0.0f, 0.0f);

    Shader::setFloat(shaderProgram, "diffuseStrength", 0.0f);
    Shader::setFloat(shaderProgram, "specularStrength", 0.0f);
    Shader::setFloat(shaderProgram, "light.constant", 1.0f);
    Shader::setFloat(shaderProgram, "light.linear", 0.0f);
    Shader::setFloat(shaderProgram, "light.quadratic", 0.0f);
}
