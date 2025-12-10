#pragma once
#include "../deps.h"
using namespace std;
namespace gllib
{
    struct DLLExport Vec2
    {
        float x = 0.f;
        float y = 0.f;
    };

    struct DLLExport UVRect
    {
        float u0 = 0.f, v0 = 0.f;
        float u1 = 0.f, v1 = 0.f;
    };
    
    // --------------------
    // Tile
    // --------------------
    struct DLLExport Tile
    {
        uint32_t gid = 0; // global id en Tiled (ya limpio de flags)
        int localId = -1; // id dentro del tileset
        Vec2 worldPos; // posición del tile en mundo
        UVRect uv; // rect UV
        bool walkable = true; // por defecto caminable

        bool empty() const { return gid == 0; }
    };
}
