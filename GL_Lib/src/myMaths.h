#pragma once

#include "transform.h"

namespace gllib
{
    class DLLExport Maths
    {
    public:
        static Quaternion Euler(glm::vec3 euler);
        static float deg2Rad();
        static glm::vec3 Quat2Vec3(Quaternion Quat, glm::vec3 Vec);
    };
}
