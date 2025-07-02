#pragma once
#include <vector>

#include "Mesh.h"
#include "ModelLoader.h"
#include "renderer.h"
#include "Entities/Entity3D.h"

namespace gllib
{
    class DLLExport Model : public Entity3D
    {
    public:
        std::vector<Mesh> meshes;
        Model(std::string const &path, bool gamma = false);

        void draw() override;
    };
}
