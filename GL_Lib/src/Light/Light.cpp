#include "Light.h"

namespace gllib
{

    Light::~Light()
    {
        lights.remove(this);
    }
}
