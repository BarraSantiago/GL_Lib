#pragma once
#include "Camera.h"
#include "input.h"
#include "lib_time.h"

namespace gllib {
    class DLLExport CameraController {
    private:
        Camera* camera;
        float moveSpeed;
        float mouseSensitivity;
        
        // Mouse tracking
        float lastX;
        float lastY;
        bool firstMouse;
        
    public:
        CameraController(Camera* camera, float moveSpeed = 5.0f, float mouseSensitivity = 0.1f);
        
        void processInput();
        void processMouseMovement(float xpos, float ypos);
        
        void setMoveSpeed(float speed) { moveSpeed = speed; }
        void setMouseSensitivity(float sensitivity) { mouseSensitivity = sensitivity; }
    };
}