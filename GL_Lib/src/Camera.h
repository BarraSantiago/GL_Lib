#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/quaternion.hpp>
#include "renderer.h"

namespace gllib {
    class Camera {
    private:
        glm::vec3 position;
        glm::vec3 front;
        glm::vec3 up;
        glm::vec3 right;
        glm::vec3 worldUp;
        
        // Euler angles
        float yaw;
        float pitch;
        
        // Perspective parameters
        float fov;
        float aspectRatio;
        float nearPlane;
        float farPlane;
        
        void updateCameraVectors();
        
    public:
        Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f), 
               glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
               float yaw = -90.0f, float pitch = 0.0f);
               
        glm::mat4 getViewMatrix() const;
        void updateViewMatrix();
        
        void setPosition(const glm::vec3& pos);
        glm::vec3 getPosition() const; 
        glm::vec3 getFront() const; 
        
        void setRotation(float newYaw, float newPitch);
        void rotate(float yawDelta, float pitchDelta);
        
        void setPerspective(float fov, float aspectRatio, float nearPlane, float farPlane);
    };
}