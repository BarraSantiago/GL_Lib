﻿#pragma once
#include "deps.h"
#include "../Maths/transform.h"

namespace gllib {
    
    class DLLExport Entity {
        
    protected:
        Transform transform;

    public:
        Entity(const Vector3& translation, const Vector3& rotationEuler, const Vector3& scale);
        Entity(const Transform& transform);
        virtual ~Entity();

        void move(const Vector3 direction);
        void rotate(const Vector3 eulerRotation);
        void updateTransform();

        Vector3 upward() const;
        Vector3 forward() const;
        Vector3 right() const;
        
        Transform getTransform() const;
        Vector3 getPosition() const;
        Vector3 getScale() const;
        Vector3 getRotationEuler() const;
        Quaternion getRotationQuat() const;

        void setTransform(const Transform& transform);
        void setPosition(const Vector3& position);
        void setScale(const Vector3& scale);
        void setRotationQuat(const Quaternion& rotation);
        void setRotationEuler(const Vector3& rotation);
        bool isColliding(const Transform& _transform) const;
        bool isColliding(float x, float y, float width, float height) const;

    };
}
