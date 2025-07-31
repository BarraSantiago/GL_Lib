#pragma once
#include <vector>

#include "Camera.h"
#include "Mesh.h"
#include "ModelLoader.h"
#include "Entities/Entity3D.h"

namespace gllib
{
    class DLLExport Model : public Entity3D
    {
    public:
        std::vector<Mesh> meshes;
        Model(std::string const& path, bool gamma);
        void draw(const Camera& camera);

        void draw() override;
    };
}
