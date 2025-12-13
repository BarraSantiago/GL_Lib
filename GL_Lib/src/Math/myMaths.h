#pragma once

#include "transform.h"

namespace gllib
{
    class DLLExport Maths
    {
    public:
        static Quaternion Euler(Vector3 euler);
        static float deg2Rad();
        static Vector3 Quat2Vec3(Quaternion Quat, Vector3 Vec);
    };
}
