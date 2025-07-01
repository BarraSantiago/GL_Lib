#pragma once
#include <string>
#include <vector>
#include "Mesh.h"
#include "../renderer.h"

namespace gllib
{
    class DLLExport Model
    {
    private:
        std::vector<Mesh> meshes;
        gllib::Renderer* renderer;
        std::string directory;

    public:
        Model(gllib::Renderer* renderer);
        ~Model();

        bool loadModel(const std::string& path, bool gamma = false);
        void draw();
        void cleanup();

        size_t getMeshCount() const { return meshes.size(); }
        const Mesh& getMesh(size_t index) const { return meshes[index]; }
    };
}