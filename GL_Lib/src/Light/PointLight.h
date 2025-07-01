#pragma once
#include "Light.h"


namespace gllib
{
    class DLLExport PointLight : public Light
    {
    private:
        Vector3 position;
        // Attenuation factors
        float constant;
        float linear;
        float quadratic;

    public:
        PointLight(const Vector3& position = {0.0f, 0.0f, 0.0f}, const Color& color = {1.0f, 1.0f, 1.0f, 1.0f},
                   float constant = 1.0f, float linear = 0.09f, float quadratic = 0.032f)
            : Light(color), position(position), constant(constant), linear(linear), quadratic(quadratic)
        {
        }

        void setPosition(const Vector3& newPosition) { position = newPosition; }
        Vector3 getPosition() const { return position; }

        void setAttenuation(float newConstant, float newLinear, float newQuadratic);

        void apply(unsigned int shaderProgram) const override;
    };
}
