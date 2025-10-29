#pragma once
#include "deps.h"
#include "glm.hpp"
#include <fwd.hpp>
#include <vec3.hpp>

class DLLExport Frustum
{
public:
    struct Plane
    {
        glm::vec3 normal;
        float distance;
        
        float distanceToPoint(const glm::vec3& point) const
        {
            return glm::dot(normal, point) + distance;
        }
    };

private:
    Plane planes[6]; // left, right, bottom, top, near, far

public:
    void extractFromMatrix(const glm::mat4& viewProjection)
    {
        // Left plane
        planes[0].normal.x = viewProjection[0][3] + viewProjection[0][0];
        planes[0].normal.y = viewProjection[1][3] + viewProjection[1][0];
        planes[0].normal.z = viewProjection[2][3] + viewProjection[2][0];
        planes[0].distance = viewProjection[3][3] + viewProjection[3][0];

        // Right plane
        planes[1].normal.x = viewProjection[0][3] - viewProjection[0][0];
        planes[1].normal.y = viewProjection[1][3] - viewProjection[1][0];
        planes[1].normal.z = viewProjection[2][3] - viewProjection[2][0];
        planes[1].distance = viewProjection[3][3] - viewProjection[3][0];

        // Bottom plane
        planes[2].normal.x = viewProjection[0][3] + viewProjection[0][1];
        planes[2].normal.y = viewProjection[1][3] + viewProjection[1][1];
        planes[2].normal.z = viewProjection[2][3] + viewProjection[2][1];
        planes[2].distance = viewProjection[3][3] + viewProjection[3][1];

        // Top plane
        planes[3].normal.x = viewProjection[0][3] - viewProjection[0][1];
        planes[3].normal.y = viewProjection[1][3] - viewProjection[1][1];
        planes[3].normal.z = viewProjection[2][3] - viewProjection[2][1];
        planes[3].distance = viewProjection[3][3] - viewProjection[3][1];

        // Near plane
        planes[4].normal.x = viewProjection[0][3] + viewProjection[0][2];
        planes[4].normal.y = viewProjection[1][3] + viewProjection[1][2];
        planes[4].normal.z = viewProjection[2][3] + viewProjection[2][2];
        planes[4].distance = viewProjection[3][3] + viewProjection[3][2];

        // Far plane
        planes[5].normal.x = viewProjection[0][3] - viewProjection[0][2];
        planes[5].normal.y = viewProjection[1][3] - viewProjection[1][2];
        planes[5].normal.z = viewProjection[2][3] - viewProjection[2][2];
        planes[5].distance = viewProjection[3][3] - viewProjection[3][2];

        // Normalize planes
        for (int i = 0; i < 6; i++)
        {
            float length = glm::length(planes[i].normal);
            planes[i].normal /= length;
            planes[i].distance /= length;
        }
    }

    bool isAABBInside(const glm::vec3& minPoint, const glm::vec3& maxPoint) const
    {
        for (int i = 0; i < 6; i++)
        {
            glm::vec3 positiveVertex = minPoint;
            if (planes[i].normal.x >= 0) positiveVertex.x = maxPoint.x;
            if (planes[i].normal.y >= 0) positiveVertex.y = maxPoint.y;
            if (planes[i].normal.z >= 0) positiveVertex.z = maxPoint.z;

            if (planes[i].distanceToPoint(positiveVertex) < 0)
                return false;
        }
        return true;
    }

    Frustum() = default;
    explicit Frustum(glm::mat<4, 4, float> mat)
    {
        extractFromMatrix(mat);
    }
};
