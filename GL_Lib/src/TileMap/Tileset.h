#pragma once
#include "Core/deps.h"
#include "TileLayer.h"

namespace gllib
{
    //TODO ADD CPP
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

        bool containsGid(uint32_t gid) const;

        int toLocalId(uint32_t gid) const;

        bool isWalkable(int localId) const;

        UVRect uvForLocalId(int localId) const;

        static Tileset LoadTSX(const std::string& tsxPath, int firstGid);
    };
}
