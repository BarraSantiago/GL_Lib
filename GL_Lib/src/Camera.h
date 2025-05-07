#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/quaternion.hpp>

#include "entity.h"
#include "renderer.h"

namespace gllib {
    class DLLExport Camera {
    private:
        glm::vec3 position;
        glm::vec3 front;
        glm::vec3 up;
        glm::vec3 right;
        glm::vec3 worldUp;

        // third person camera
        gllib::Entity* target = nullptr;
        float distance = 5.0f;
        float height = 2.0f;
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
        glm::mat4 getProjectionMatrix() const;
        
        void setRotation(float newYaw, float newPitch);
        void rotate(float yawDelta, float pitchDelta);
        
        void setPerspective(float fov, float aspectRatio, float nearPlane, float farPlane);

        void setTarget(gllib::Entity* targetEntity) { target = targetEntity; }
        void setDistance(float dist) { distance = dist; }
        void setHeight(float h) { height = h; }
        void updateThirdPersonPosition();
    };
}