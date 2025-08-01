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
        AmbientLight(const Color& color = {1.0f, 1.0f, 1.0f, 1.0f}, float intensity = 0.1f);

        void setIntensity(float newIntensity);
        float getIntensity() const;

        void apply(unsigned int shaderProgram) const override;
    };
}
