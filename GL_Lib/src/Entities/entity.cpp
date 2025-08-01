#include "entity.h"


#include "shape.h"

namespace gllib
{
    Entity::Entity(const glm::vec3& translation, const glm::vec3& rotationEuler, const glm::vec3& scale)
    {
        transform.position = translation;
        transform.rotationQuat = Maths::Euler(rotationEuler);
        transform.scale = scale;
    }

    Entity::Entity(const Transform& transform): transform(transform)
    {
    }

    Entity::~Entity()
    = default;

    void Entity::move(const glm::vec3 direction)
    {
        transform.position += direction;
    }

    void Entity::rotate(const glm::vec3 eulerRotation)
    {
        Quaternion rotationQuat;
        rotationQuat.x = eulerRotation.x;
        rotationQuat.y = eulerRotation.y;
        rotationQuat.z = eulerRotation.z;
        transform.rotationQuat += rotationQuat;
    }

    void Entity::updateTransform()
    {
        transform.forward = Maths::Quat2Vec3(transform.rotationQuat, glm::vec3(0, 0, 1));
        transform.upward = Maths::Quat2Vec3(transform.rotationQuat, glm::vec3(0, 1, 0));
        transform.right = Maths::Quat2Vec3(transform.rotationQuat, glm::vec3(1, 0, 0));
    }

    glm::vec3 Entity::upward() const
    {
        return transform.upward;
    }

    glm::vec3 Entity::forward() const
    {
        return transform.forward;
    }

    glm::vec3 Entity::right() const
    {
        return transform.right;
    }

    Transform Entity::getTransform() const
    {
        return transform;
    }

    glm::vec3 Entity::getPosition() const
    {
        return transform.position;
    }

    glm::vec3 Entity::getScale() const
    {
        return transform.scale;
    }

    glm::vec3 Entity::getRotationEuler() const
    {
        return Maths::Quat2Vec3(transform.rotationQuat, glm::vec3(1, 1, 1));
    }

    Quaternion Entity::getRotationQuat() const
    {
        return transform.rotationQuat;
    }

    void Entity::setTransform(const Transform& transform)
    {
        this->transform = transform;
    }

    void Entity::setPosition(const glm::vec3& position)
    {
        transform.position = position;
    }

    void Entity::setScale(const glm::vec3& scale)
    {
        transform.scale = scale;
    }

    void Entity::setRotationQuat(const Quaternion& rotation)
    {
        
        transform.rotationQuat =  rotation;
    }

    void Entity::setRotationEuler(const glm::vec3& rotation)
    {
        transform.rotationQuat = Maths::Euler(rotation);
    }

    bool Entity::isColliding(const Transform& _transform) const
    {
        float xOffset = 0.5f * transform.scale.x;
        float yOffset = 0.5f * transform.scale.y;

        float thisAdjustedX = transform.position.x - xOffset;
        float thisAdjustedY = transform.position.y - yOffset;

        xOffset = 0.5f * _transform.scale.x;
        yOffset = 0.5f * _transform.scale.y;

        float otherAdjustedX = _transform.position.x - xOffset;
        float otherAdjustedY = _transform.position.y - yOffset;

        if (thisAdjustedX + transform.scale.x >= otherAdjustedX &&
            thisAdjustedX <= otherAdjustedX + _transform.scale.x &&
            thisAdjustedY + transform.scale.y >= otherAdjustedY &&
            thisAdjustedY <= otherAdjustedY + _transform.scale.y)
        {
            return true;
        }

        return false;
    }

    bool Entity::isColliding(float x, float y, float width, float height) const
    {
        if (transform.position.x + transform.scale.x >= x &&
            transform.position.x <= x + width &&
            transform.position.y + transform.scale.y >= y &&
            transform.position.y <= y + height)
        {
            return true;
        }
        return false;
    }

    glm::mat4 Entity::getModelMatrix() const
    {
        glm::mat4 model = glm::mat4(1.0f);

        model = glm::translate(model, glm::vec3(transform.position.x, transform.position.y, transform.position.z));

        glm::quat glmQuat(transform.rotationQuat.w, transform.rotationQuat.x, transform.rotationQuat.y, transform.rotationQuat.z);
        glm::mat4 rotationMatrix = glm::mat4_cast(glmQuat);
        model = model * rotationMatrix;

        model = glm::scale(model, glm::vec3(transform.scale.x, transform.scale.y, transform.scale.z));

        return model;
    }
}
