#pragma once
#include "../Core/deps.h"
#include "TileLayer.h"

namespace gllib
{
    // --------------------
    // Tileset (TSX)
    // --------------------
    class DLLExport Tileset
    {
    public:
        int firstGid = 1;

        int tileWidth = 0;
        int tileHeight = 0;
        int columns = 0;
        int tileCount = 0;

        int imageWidth = 0;
        int imageHeight = 0;
        std::string imageSource;

        std::unordered_map<int, bool> walkableOverride;

        bool containsGid(uint32_t gid) const
        {
            if (gid == 0) return false;
            const uint32_t local = gid - firstGid;
            return local < static_cast<uint32_t>(tileCount);
        }

        int toLocalId(uint32_t gid) const
        {
            return static_cast<int>(gid - firstGid);
        }

        bool isWalkable(int localId) const
        {
            auto it = walkableOverride.find(localId);
            if (it != walkableOverride.end()) return it->second;
            return true;
        }

        UVRect uvForLocalId(int localId) const
        {
            if (columns <= 0 || imageWidth <= 0 || imageHeight <= 0)
                throw std::runtime_error("Tileset inválido para calcular UV.");

            int tx = localId % columns;
            int ty = localId / columns;

            float px0 = static_cast<float>(tx * tileWidth);
            float py0 = static_cast<float>(ty * tileHeight);
            float px1 = px0 + tileWidth;
            float py1 = py0 + tileHeight;

            UVRect r;
            r.u0 = px0 / imageWidth;
            r.v0 = py0 / imageHeight;
            r.u1 = px1 / imageWidth;
            r.v1 = py1 / imageHeight;
            return r;
        }

        static Tileset LoadTSX(const std::string& tsxPath, int firstGid);
    };
}
