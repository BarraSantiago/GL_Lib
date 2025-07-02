#pragma once
#include "Light.h"


namespace gllib
{
    class DLLExport PointLight : public Light
    {
    private:
        glm::vec3 position;
        // Attenuation factors
        float constant;
        float linear;
        float quadratic;

    public:
        PointLight(const glm::vec3& position = {0.0f, 0.0f, 0.0f}, const Color& color = {1.0f, 1.0f, 1.0f, 1.0f},
                   float constant = 1.0f, float linear = 0.09f, float quadratic = 0.032f)
            : Light(color), position(position), constant(constant), linear(linear), quadratic(quadratic)
        {
        }

        void setPosition(const glm::vec3& newPosition) { position = newPosition; }
        glm::vec3 getPosition() const { return position; }

        void setAttenuation(float newConstant, float newLinear, float newQuadratic)
        {
            constant = newConstant;
            linear = newLinear;
            quadratic = newQuadratic;
        }

        void apply(unsigned int shaderProgram) const override
        {
            Shader::setVec3(shaderProgram, "lightPos", position.x, position.y, position.z);
            Shader::setVec3(shaderProgram, "lightColor", color.r, color.g, color.b);

            Shader::setFloat(shaderProgram, "diffuseStrength", 1.0f);
            Shader::setFloat(shaderProgram, "specularStrength", 0.5f);

            Shader::setFloat(shaderProgram, "light.constant", constant);
            Shader::setFloat(shaderProgram, "light.linear", linear);
            Shader::setFloat(shaderProgram, "light.quadratic", quadratic);
        }
    };
}
