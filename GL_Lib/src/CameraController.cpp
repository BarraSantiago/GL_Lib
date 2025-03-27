#include "CameraController.h"

namespace gllib {
    CameraController::CameraController(Camera* camera, float moveSpeed, float mouseSensitivity)
        : camera(camera), moveSpeed(moveSpeed), mouseSensitivity(mouseSensitivity),
          lastX(0.0f), lastY(0.0f), firstMouse(true) {
    }
    
    void CameraController::processInput() {
        float velocity = moveSpeed * LibTime::getDeltaTime();
        glm::vec3 position = camera->getPosition();
        glm::vec3 front = camera->getFront();
        
        // WASD movement
        if (Input::getKeyPressed(Key_W))
            position += front * velocity;
        if (Input::getKeyPressed(Key_S))
            position -= front * velocity;
        if (Input::getKeyPressed(Key_A))
            position -= glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f))) * velocity;
        if (Input::getKeyPressed(Key_D))
            position += glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f))) * velocity;
        
        camera->setPosition(position);
    }
    
    void CameraController::processMouseMovement(float xpos, float ypos) {
        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }
        
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // Reversed: y ranges bottom to top
        
        lastX = xpos;
        lastY = ypos;
        
        xoffset *= mouseSensitivity;
        yoffset *= mouseSensitivity;
        
        camera->rotate(xoffset, yoffset);
    }
}