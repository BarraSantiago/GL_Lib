#pragma once
#include <ext/matrix_transform.hpp>

#include "deps.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "gtx/quaternion.hpp"
#ifdef _WIN32
#include "glm.hpp"
#else
#include "glm/glm.hpp"
#endif // _WIN32

namespace gllib
{
    struct DLLExport Quaternion
    {
        float w;
        float x;
        float y;
        float z;

        Quaternion& operator+=(const Quaternion& quaternion)
        {
            w += quaternion.w;
            x += quaternion.x;
            y += quaternion.y;
            z += quaternion.z;
            return *this;
        }

        void normalize()
        {
            float magnitude = std::sqrt(w * w + x * x + y * y + z * z);

            // Avoid division by zero
            if (magnitude > 0.0000001f)
            {
                w /= magnitude;
                x /= magnitude;
                y /= magnitude;
                z /= magnitude;
            }
            else
            {
                w = 1.0f;
                x = 0.0f;
                y = 0.0f;
                z = 0.0f;
            }
        }
    };

    struct DLLExport Color
    {
        float r;
        float g;
        float b;
        float a;

        void normalize()
        {
            if (r < 0 || g < 0 || b < 0 || a < 0)
            {
                r = 0.0f;
                g = 0.0f;
                b = 0.0f;
                a = 0.0f;
            }

            if (r > 1.0f || g > 1.0f || b > 1.0f || a > 1.0f)
            {
                r = 1.0f;
                g = 1.0f;
                b = 1.0f;
                a = 1.0f;
            }
        }
    };

    struct DLLExport Transform
    {
        glm::vec3 position;
        glm::vec3 scale;
        Quaternion rotationQuat;

        glm::vec3 forward;
        glm::vec3 upward;
        glm::vec3 right;

        // Hierarchy support
        Transform* parent = nullptr;
        std::vector<Transform*> children;

        // AABB for frustum culling
        glm::vec3 aabbMin = glm::vec3(0.0f);
        glm::vec3 aabbMax = glm::vec3(0.0f);

        void addChild(Transform* child)
        {
            if (child && child->parent != this)
            {
                if (child->parent)
                    child->parent->removeChild(child);
                child->parent = this;
                children.push_back(child);
            }
        }

        void removeChild(Transform* child)
        {
            auto it = std::find(children.begin(), children.end(), child);
            if (it != children.end())
            {
                (*it)->parent = nullptr;
                children.erase(it);
            }
        }

        Transform operator/(float i)
        {
            return {
                position / i,
                scale / i,
                {rotationQuat.w / i, rotationQuat.x / i, rotationQuat.y / i, rotationQuat.z / i},
                forward / i,
                upward / i,
                right / i
            };
        }

        Transform operator*(float i)
        {
            return {
                position * i,
                scale * i,
                {rotationQuat.w * i, rotationQuat.x * i, rotationQuat.y * i, rotationQuat.z * i},
                forward * i,
                upward * i,
                right * i
            };
        }

        glm::mat4 getTransformMatrix()
        {
            glm::mat4 localMatrix = getLocalTransformMatrix();
            if (parent)
                return parent->getTransformMatrix() * localMatrix;
            return localMatrix;
        }

        glm::mat4 getLocalTransformMatrix()
        {
            glm::mat4 translation = glm::translate(glm::mat4(1.0f), position);
            glm::mat4 scaling = glm::scale(glm::mat4(1.0f), scale);

            glm::quat rotation = glm::quat(rotationQuat.w, rotationQuat.x, rotationQuat.y, rotationQuat.z);
            glm::mat4 rotationMatrix = glm::mat4(rotation);

            return translation * rotationMatrix * scaling;
        }

        glm::vec3 getWorldAABBMin() const
        {
            glm::mat4 worldMatrix = const_cast<Transform*>(this)->getTransformMatrix();
            glm::vec4 worldMin = worldMatrix * glm::vec4(aabbMin, 1.0f);
            return glm::vec3(worldMin);
        }

        glm::vec3 getWorldAABBMax() const
        {
            glm::mat4 worldMatrix = const_cast<Transform*>(this)->getTransformMatrix();
            glm::vec4 worldMax = worldMatrix * glm::vec4(aabbMax, 1.0f);
            return glm::vec3(worldMax);
        }
    };;

    struct DLLExport ModelMatrix
    {
        glm::mat4 model;
        glm::mat4 translate;
        glm::mat4 rotationX;
        glm::mat4 rotationY;
        glm::mat4 rotationZ;
        glm::mat4 scale;
    };
}
