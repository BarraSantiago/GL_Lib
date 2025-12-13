#include "Tile.h"

namespace gllib
{
    Tile::Tile() : Sprite(Transform(), {1.0f, 1.0f, 1.0f, 1.0f})
    {
        gid = 0;
        localId = -1;
        walkable = true;
    }

    Tile::Tile(uint32_t gid, int localId, Vec2 worldPos, UVRect uv, bool walkable,
               int tileWidth, int tileHeight, unsigned int textureID)
        : Sprite({Vector3(worldPos.x, worldPos.y, 0.0f),
                  Vector3(static_cast<float>(tileWidth), static_cast<float>(tileHeight), 1.0f)},
                 {1.0f, 1.0f, 1.0f, 1.0f})
    {
        this->gid = gid;
        this->localId = localId;
        this->walkable = walkable;
    
        if (textureID != 0)
        {
            int textureWidth = 0, textureHeight = 0;
            Renderer::getTextureSize(textureID, &textureWidth, &textureHeight);
            
            int pixelX = static_cast<int>(uv.u0 * textureWidth);
            int pixelY = static_cast<int>(uv.v0 * textureHeight);
            
            addFrame(textureID, pixelX, pixelY, tileWidth, tileHeight);
        }
    }

    void Tile::draw()
    {
        updateRenderData();
        Sprite::draw();
    }
}