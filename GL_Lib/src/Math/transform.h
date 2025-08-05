#pragma once
#include <ext/matrix_transform.hpp>

#include "Core/deps.h"
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

        // Local AABB (model space)
        glm::vec3 localAABBMin = glm::vec3(0.0f);
        glm::vec3 localAABBMax = glm::vec3(0.0f);

        // Hierarchical AABB (world space, includes children)
        glm::vec3 hierarchicalAABBMin = glm::vec3(0.0f);
        glm::vec3 hierarchicalAABBMax = glm::vec3(0.0f);

        // Cached transform matrix
        mutable glm::mat4 cachedWorldMatrix = glm::mat4(1.0f);
        mutable bool worldMatrixDirty = true;

        void setPosition(const glm::vec3& newPosition)
        {
            position = newPosition;
            markDirty();
        }

        void setScale(const glm::vec3& newScale)
        {
            scale = newScale;
            markDirty();
        }

        void setRotation(const Quaternion& newRotation)
        {
            rotationQuat = newRotation;
            markDirty();
        }

        void markDirty()
        {
            worldMatrixDirty = true;
            for (Transform* child : children)
            {
                child->markDirty();
            }
        }

        void addChild(Transform* child)
        {
            if (child && child->parent != this)
            {
                if (child->parent)
                    child->parent->removeChild(child);
                child->parent = this;
                children.push_back(child);
                child->markDirty();
            }
        }

        void removeChild(Transform* child)
        {
            auto it = std::find(children.begin(), children.end(), child);
            if (it != children.end())
            {
                (*it)->parent = nullptr;
                children.erase(it);
                (*it)->markDirty();
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

        glm::mat4 getTransformMatrix() const
        {
            if (worldMatrixDirty)
            {
                updateWorldMatrix();
            }
            return cachedWorldMatrix;
        }

        void updateWorldMatrix() const
        {
            glm::mat4 localMatrix = getLocalTransformMatrix();
            if (parent)
            {
                cachedWorldMatrix = parent->getTransformMatrix() * localMatrix;
            }
            else
            {
                cachedWorldMatrix = localMatrix;
            }
            worldMatrixDirty = false;
        }

        glm::mat4 getLocalTransformMatrix() const
        {
            glm::mat4 translation = glm::translate(glm::mat4(1.0f), position);
            glm::mat4 scaling = glm::scale(glm::mat4(1.0f), scale);

            glm::quat rotation = glm::quat(rotationQuat.w, rotationQuat.x, rotationQuat.y, rotationQuat.z);
            glm::mat4 rotationMatrix = glm::mat4(rotation);

            return translation * rotationMatrix * scaling;
        }

        // Top-down: Update world matrices recursively
        void updateTransformHierarchy()
        {
            updateWorldMatrix();
            for (Transform* child : children)
            {
                child->updateTransformHierarchy();
            }
        }

        // Bottom-up: Calculate hierarchical AABB including all children
        void calculateHierarchicalAABB()
        {
            glm::mat4 worldMatrix = getTransformMatrix();
            
            // Reset hierarchical AABB
            hierarchicalAABBMin = glm::vec3(FLT_MAX);
            hierarchicalAABBMax = glm::vec3(-FLT_MAX);
            
            bool hasValidAABB = false;
        
            // Only process local AABB if it's valid (not zero-sized)
            if (localAABBMin != localAABBMax)
            {
                // Transform all 8 corners of the local AABB
                std::vector<glm::vec3> corners = {
                    glm::vec3(localAABBMin.x, localAABBMin.y, localAABBMin.z),
                    glm::vec3(localAABBMax.x, localAABBMin.y, localAABBMin.z),
                    glm::vec3(localAABBMin.x, localAABBMax.y, localAABBMin.z),
                    glm::vec3(localAABBMax.x, localAABBMax.y, localAABBMin.z),
                    glm::vec3(localAABBMin.x, localAABBMin.y, localAABBMax.z),
                    glm::vec3(localAABBMax.x, localAABBMin.y, localAABBMax.z),
                    glm::vec3(localAABBMin.x, localAABBMax.y, localAABBMax.z),
                    glm::vec3(localAABBMax.x, localAABBMax.y, localAABBMax.z)
                };
        
                for (const glm::vec3& corner : corners)
                {
                    glm::vec4 worldCorner = worldMatrix * glm::vec4(corner, 1.0f);
                    glm::vec3 worldPos = glm::vec3(worldCorner);
                    
                    if (!hasValidAABB)
                    {
                        hierarchicalAABBMin = worldPos;
                        hierarchicalAABBMax = worldPos;
                        hasValidAABB = true;
                    }
                    else
                    {
                        hierarchicalAABBMin = glm::min(hierarchicalAABBMin, worldPos);
                        hierarchicalAABBMax = glm::max(hierarchicalAABBMax, worldPos);
                    }
                }
            }
        
            // Recursively calculate children AABBs and include them
            for (Transform* child : children)
            {
                child->calculateHierarchicalAABB();
                
                // Include child AABB if it's valid
                if (child->hierarchicalAABBMin.x <= child->hierarchicalAABBMax.x &&
                    child->hierarchicalAABBMin.y <= child->hierarchicalAABBMax.y &&
                    child->hierarchicalAABBMin.z <= child->hierarchicalAABBMax.z)
                {
                    if (!hasValidAABB)
                    {
                        hierarchicalAABBMin = child->hierarchicalAABBMin;
                        hierarchicalAABBMax = child->hierarchicalAABBMax;
                        hasValidAABB = true;
                    }
                    else
                    {
                        hierarchicalAABBMin = glm::min(hierarchicalAABBMin, child->hierarchicalAABBMin);
                        hierarchicalAABBMax = glm::max(hierarchicalAABBMax, child->hierarchicalAABBMax);
                    }
                }
            }
        
            // If no valid AABB was found, set a minimal one at world position
            if (!hasValidAABB)
            {
                glm::vec3 worldPos = glm::vec3(worldMatrix[3]);
                hierarchicalAABBMin = worldPos - glm::vec3(0.1f);
                hierarchicalAABBMax = worldPos + glm::vec3(0.1f);
            }
        }

        glm::vec3 getWorldAABBMin() const
        {
            return hierarchicalAABBMin;
        }

        glm::vec3 getWorldAABBMax() const
        {
            return hierarchicalAABBMax;
        }
    };;;

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
