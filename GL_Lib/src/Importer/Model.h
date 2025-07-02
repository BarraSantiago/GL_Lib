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
        // constructor, expects a filepath to a 3D model.
        Model(std::string const &path, bool gamma = false);

        // draws the model, and thus all its meshes
        void draw() override;
    };
}
