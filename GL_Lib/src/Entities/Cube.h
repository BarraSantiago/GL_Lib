#pragma once
#include "entity.h"
#include "../Rendering/Light/Material.h"

namespace gllib {
    class DLLExport Cube : public Entity {
    private:
        unsigned int VAO, VBO, EBO;
        glm::vec3 color;
        Material* material;
        bool ownsMaterial;

    public:
        // Constructor with material
        Cube(Transform transform, Material* material);
        
        // Backward compatibility constructor (converts color to material)
        Cube(Transform transform, glm::vec4 color);
        
        ~Cube() override;

        void draw();
        void setMaterial(Material* material, bool takeOwnership = false);
        Material* getMaterial() const { return material; }
    };
}