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
                   float constant = 1.0f, float linear = 0.09f, float quadratic = 0.032f);

        glm::vec3 getPosition() const;
        void setPosition(const glm::vec3& newPosition);
        void setAttenuation(float newConstant, float newLinear, float newQuadratic);
        void apply(unsigned int shaderProgram) const override;
    };
}
