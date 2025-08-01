#pragma once
#include "Entity2.h"
#include "Material.h"

namespace gllib
{
    class DLLExport Entity3D : public Entity2
    {
    protected:
        Material* material;
        float* normals;
    
    public:
        explicit Entity3D();
        ~Entity3D() override;
    
        void draw() override;
        void updateVao() override;

        void setMaterial(Material* material);
    };
}
