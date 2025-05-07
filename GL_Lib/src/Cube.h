#pragma once
#include "entity.h"

namespace gllib {
    class DLLExport Cube : public Entity {
    private:
        unsigned int VAO, VBO, EBO;
        glm::vec3 color;
    
    public:
        Cube(Transform transform, glm::vec4 color);
        ~Cube() override;
        
        void draw();
    };
}