#pragma once
#include "../Entities/sprite.h"

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
    
    class DLLExport Tile : public Sprite
    {
        public:
        uint32_t gid = 0;
        int localId = -1;
        bool walkable = true;

        Tile();
        Tile(uint32_t gid, int localId, Vec2 worldPos, UVRect uv, bool walkable, int tileWidth, int tileHeight, unsigned int textureID);
        
        void draw() override;
        bool empty() const { return gid == 0; }
        void setUVCoords(const UVRect& uv);
    };
}