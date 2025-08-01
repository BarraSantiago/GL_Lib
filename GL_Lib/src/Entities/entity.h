#pragma once
#include "../transform.h"
#include "../deps.h"
#include "../renderer.h"
#include "../Material.h"
#include "../myMaths.h"
#include "../renderer.h"

namespace gllib
{
    class DLLExport Entity
    {
    protected:
        Transform transform;

    public:
        Entity(const glm::vec3& translation, const glm::vec3& rotationEuler, const glm::vec3& scale);
        Entity(const Transform& transform);
        virtual ~Entity();

        void move(const glm::vec3 direction);
        void rotate(const glm::vec3 eulerRotation);
        void updateTransform();

        glm::vec3 upward() const;
        glm::vec3 forward() const;
        glm::vec3 right() const;

        Transform getTransform() const;
        glm::vec3 getPosition() const;
        glm::vec3 getScale() const;
        glm::vec3 getRotationEuler() const;
        Quaternion getRotationQuat() const;

        void setTransform(const Transform& transform);
        void setPosition(const glm::vec3& position);
        void setScale(const glm::vec3& scale);
        void setRotationQuat(const Quaternion& rotation);
        void setRotationEuler(const glm::vec3& rotation);
        bool isColliding(const Transform& _transform) const;
        bool isColliding(float x, float y, float width, float height) const;
        glm::mat4 getModelMatrix() const;
    };
}
