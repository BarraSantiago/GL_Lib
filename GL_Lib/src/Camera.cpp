#include "Camera.h"

namespace gllib
{
    Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
        : position(position), worldUp(up), yaw(yaw), pitch(pitch),
          fov(45.0f), aspectRatio(4.0f / 3.0f), nearPlane(0.1f), farPlane(100.0f)
    {
        updateCameraVectors();
    }

    void Camera::updateCameraVectors()
    {
        // Calculate the front vector
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(front);

        // Recalculate the right and up vector
        right = glm::normalize(glm::cross(front, worldUp));
        up = glm::normalize(glm::cross(right, front));

        // Update the view matrix in the renderer
        updateViewMatrix();
    }

    glm::mat4 Camera::getViewMatrix() const
    {
        return glm::lookAt(position, position + front, up);
    }

    void Camera::updateViewMatrix()
    {
        Renderer::setViewMatrix(getViewMatrix());
    }

    void Camera::setPosition(const glm::vec3& pos)
    {
        position = pos;
        updateViewMatrix();
    }

    glm::vec3 Camera::getPosition() const
    {
        return position;
    }

    glm::vec3 Camera::getFront() const
    {
        return front;
    }

    glm::mat4 Camera::getProjectionMatrix() const {
        return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
    }

    void Camera::setRotation(float newYaw, float newPitch)
    {
        yaw = newYaw;
        // Constrain pitch
        pitch = std::max(-89.0f, std::min(89.0f, newPitch)); 
        updateCameraVectors();
    }

    void Camera::rotate(float yawDelta, float pitchDelta)
    {
        yaw += yawDelta;
        pitch += pitchDelta;
        // Constrain pitch
        pitch = std::max(-89.0f, std::min(89.0f, pitch));
        updateCameraVectors();
    }

    void Camera::setPerspective(float fov, float aspectRatio, float nearPlane, float farPlane)
    {
        this->fov = fov;
        this->aspectRatio = aspectRatio;
        this->nearPlane = nearPlane;
        this->farPlane = farPlane;
        Renderer::setPerspectiveProjectionMatrix(fov, aspectRatio, nearPlane, farPlane);
    }

    void Camera::updateThirdPersonPosition()
    {
        if (!target) return;
        
        glm::vec3 targetPos = glm::vec3(target->getPosition().x, target->getPosition().y, target->getPosition().z);
        
        // Calculate camera position behind target
        glm::vec3 offset = -front * distance;
        offset.y += height;
        
        position = targetPos + offset;
        updateViewMatrix();
    }

    void Camera::invertYAxis()
    {
        worldUp.y = -worldUp.y;
        updateCameraVectors();
    }
}
