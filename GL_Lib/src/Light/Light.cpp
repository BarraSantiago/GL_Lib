#include "Light.h"

namespace gllib
{
    Light::Light(const Color& color): color(color)
    {
        lights.push_back(this);
    }

    Light::~Light()
    {
        lights.remove(this);
    }

    void Light::setColor(const Color& newColor)
    {
        color = newColor;
    }

    Color Light::getColor() const
    {
        return color;
    }
}
