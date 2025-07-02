#pragma once
#include "Light.h"

namespace gllib
{
    class DLLExport SpotLight : public Light
    {
    private:
        glm::vec3 position;
        glm::vec3 direction;

        float innerCutOff;
        float outerCutOff;

        float constant;
        float linear;
        float quadratic;

    public:
        SpotLight(const glm::vec3& position = {0.0f, 0.0f, 0.0f},
                  const glm::vec3& direction = {0.0f, -1.0f, 0.0f},
                  const Color& color = {1.0f, 1.0f, 1.0f, 1.0f},
                  float innerCutOff = 12.5f, float outerCutOff = 15.0f,
                  float constant = 1.0f, float linear = 0.09f, float quadratic = 0.032f);

        glm::vec3 getPosition() const;
        void setPosition(const glm::vec3& newPosition);

        glm::vec3 getDirection() const;
        void setDirection(const glm::vec3& newDirection);

        void setCutOff(float newInnerCutOff, float newOuterCutOff);
        float getInnerCutOff() const;
        float getOuterCutOff() const;

        void setAttenuation(float newConstant, float newLinear, float newQuadratic);

        void apply(unsigned int shaderProgram) const override;
    };
}
