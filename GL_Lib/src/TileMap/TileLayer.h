#pragma once
#include <string>
#include <vector>

#include "Tile.h"
#include "../Core/deps.h"

namespace gllib
{
    // --------------------
    // Layer
    // --------------------
    struct DLLExport TileLayer
    {
        std::string name;
        int width = 0;
        int height = 0;
        bool visible = true;

        // tiles[row][col]
        std::vector<std::vector<Tile>> tiles;
    };
}
