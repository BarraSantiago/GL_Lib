#pragma once
#include "Core/deps.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <stdexcept>
#include <cmath>
#include <algorithm>

#include "Tileset.h"
#include "tinyxml2.h"
#include "Rendering/renderer.h"
namespace gllib
{
    // --------------------
    // Tipos simples
    // --------------------
    



    // --------------------
    // Tilemap
    // --------------------
    class DLLExport TileMap
    {
    public:
        int width = 0; // en tiles
        int height = 0; // en tiles
        int tileWidth = 0; // px
        int tileHeight = 0; // px

        std::vector<Tileset> tilesets;
        std::vector<TileLayer> layers;

        static TileMap LoadFromTiledXML(const std::string& tmxPath, unsigned int atlasTextureID);

        void draw() const;

        bool checkCollisionAABB(Transform transform) const;

    private:
        const Tileset* findTilesetForGid(uint32_t gid) const;

        static uint32_t ClearGidFlags(uint32_t gid);
    };

    std::string ReadTextFile(const std::string& path);
}
