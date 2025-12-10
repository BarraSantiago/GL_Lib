#pragma once
#include "../deps.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <stdexcept>
#include <cmath>
#include <algorithm>

#include "Tileset.h"
#include "tinyxml2.h"
#include "../renderer.h"
namespace gllib
{
    // --------------------
    // Tipos simples
    // --------------------
    

    struct DLLExport Rect
    {
        float x = 0.f, y = 0.f;
        float w = 0.f, h = 0.f;
    };

    inline bool Intersects(const Rect& a, const Rect& b)
    {
        return !(a.x + a.w <= b.x ||
            b.x + b.w <= a.x ||
            a.y + a.h <= b.y ||
            b.y + b.h <= a.y);
    }

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

        static TileMap LoadFromTiledXML(const std::string& tmxPath);

        void draw(unsigned int atlasTextureID) const;

        bool checkCollisionAABB(float x, float y, float w, float h) const;

    private:
        const Tileset* findTilesetForGid(uint32_t gid) const;

        static uint32_t ClearGidFlags(uint32_t gid);
    };

    // Utilidad simple para leer archivo a string
    std::string ReadTextFile(const std::string& path);
}
