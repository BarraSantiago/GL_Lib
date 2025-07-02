#include "CameraController.h"

namespace gllib
{
    CameraController::CameraController(Camera* camera, float moveSpeed, float mouseSensitivity)
        : camera(camera), moveSpeed(moveSpeed), mouseSensitivity(mouseSensitivity),
          lastX(0.0f), lastY(0.0f), firstMouse(true), currentMode(CameraMode::ThirdPerson), toggleKeyPressed(false)
    {
    }

    void CameraController::processInput()
    {
        // Toggle camera mode with C key
        if (Input::getKeyPressed(Key_C))
        {
            if (!toggleKeyPressed)
            {
                currentMode = (currentMode == CameraMode::FirstPerson)
                                  ? CameraMode::ThirdPerson
                                  : CameraMode::FirstPerson;
                toggleKeyPressed = true;
            }
        }
        else
        {
            toggleKeyPressed = false;
        }

        // Only process WASD movement in first person mode
        if (currentMode == CameraMode::FirstPerson)
        {
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
        // In third person mode, the camera position is handled by updateThirdPersonPosition()
    }

    void CameraController::processMouseMovement(float xpos, float ypos)
    {
        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;

        lastX = xpos;
        lastY = ypos;

        xoffset *= mouseSensitivity;
        yoffset *= mouseSensitivity;

        camera->rotate(xoffset, yoffset);
    }
}
