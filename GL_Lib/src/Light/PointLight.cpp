#include "PointLight.h"

void gllib::PointLight::setAttenuation(float newConstant, float newLinear, float newQuadratic)
{
    constant = newConstant;
    linear = newLinear;
    quadratic = newQuadratic;
}

void gllib::PointLight::apply(unsigned int shaderProgram) const
{
    Shader::setVec3(shaderProgram, "lightPos", position.x, position.y, position.z);
    Shader::setVec3(shaderProgram, "lightColor", color.r, color.g, color.b);

    Shader::setFloat(shaderProgram, "diffuseStrength", 1.0f);
    Shader::setFloat(shaderProgram, "specularStrength", 0.5f);

    Shader::setFloat(shaderProgram, "light.constant", constant);
    Shader::setFloat(shaderProgram, "light.linear", linear);
    Shader::setFloat(shaderProgram, "light.quadratic", quadratic);
}
