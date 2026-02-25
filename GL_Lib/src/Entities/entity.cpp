#include "entity.h"

#include <vector>

#include "Math/myMaths.h"

namespace gllib
{
    Entity::Entity(const Vector3& translation, const Vector3& rotationEuler, const Vector3& scale) {
        transform.position = translation;
        transform.rotationQuat = Maths::Euler(rotationEuler);
        transform.scale = scale;
    }

    Entity::Entity(const Transform& transform): transform(transform)
    {
    }

    Entity::~Entity()
    = default;

    void Entity::move(const Vector3 direction) {
        transform.position += direction;
        updateModelMatrix();
    }

    void Entity::rotate(const Vector3 eulerRotation) {
        Quaternion rotationQuat;
        rotationQuat.x = eulerRotation.x;
        rotationQuat.y = eulerRotation.y;
        rotationQuat.z = eulerRotation.z;
        transform.rotationQuat += rotationQuat;
        updateModelMatrix();
    }

    void Entity::updateTransform() {
        transform.forward = Maths::Quat2Vec3(transform.rotationQuat, Vector3(0, 0, 1));
        transform.upward = Maths::Quat2Vec3(transform.rotationQuat, Vector3(0, 1, 0));
        transform.right = Maths::Quat2Vec3(transform.rotationQuat, Vector3(1, 0, 0));
        updateModelMatrix();
    }

    Vector3 Entity::upward() const {
        return transform.upward;
    }

    Vector3 Entity::forward() const {
        return transform.forward;
    }

    Vector3 Entity::right() const {
        return transform.right;
    }

    Transform Entity::getTransform() const
    {
        return transform;
    }

    Vector3 Entity::getPosition() const
    {
        return transform.position;
    }

    Vector3 Entity::getScale() const
    {
        return transform.scale;
    }

    Vector3 Entity::getRotationEuler() const
    {
        return Maths::Quat2Vec3( transform.rotationQuat, Vector3(1, 1, 1));
    }

    Quaternion Entity::getRotationQuat() const
    {
        return transform.rotationQuat;
    }

    void Entity::setTransform(const Transform& transform) {
        this->transform = transform;
        updateModelMatrix();
    }

    void Entity::setPosition(const Vector3& position) {
        transform.position = position;
        updateModelMatrix();
    }

    void Entity::setScale(const Vector3& scale) {
        transform.scale = scale;
        updateModelMatrix();
    }

    void Entity::setRotationQuat(const Quaternion& rotation) {
        transform.rotationQuat = rotation;
        updateModelMatrix();
    }

    void Entity::setRotationEuler(const Vector3& rotation)	{
        transform.rotationQuat = Maths::Euler(rotation);
        updateModelMatrix();
    }
}
