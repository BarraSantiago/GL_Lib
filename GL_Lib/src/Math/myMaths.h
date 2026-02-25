#pragma once

#include "transform.h"

namespace gllib
{
    class DLLExport Maths
    {
    public:
        struct Rect
        {
            float x = 0.f, y = 0.f;
            float w = 0.f, h = 0.f;
        };

        static Quaternion Euler(Vector3 euler);
        static float deg2Rad();
        static Vector3 Quat2Vec3(Quaternion Quat, Vector3 Vec);
        static bool checkAABB(const Rect& a, const Rect& b);
        static bool checkAABB(const Transform& transform, const Transform& _transform);
        static bool checkAABB(float x1, float y1, float width1, float height1, float x2, float y2, float width2, float height2);
    };
}
