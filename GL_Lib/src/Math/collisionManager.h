#pragma once
#include <vector>

#include "../Rendering/renderer.h"
#include "../Entities/rectangle.h"
#include "../TileMap/TileMap.h"

namespace gllib
{
    class DLLExport collisionManager
    {
    public:
        collisionManager(const std::vector<Entity*>& obstacles, TileMap* tilemap = nullptr);
        ~collisionManager();
        bool checkCollision(Entity* player);
        bool checkCollision(Transform transform);
        void addObstacle(Entity* obstacle);
        void removeObstacle(Entity* obstacle_);
        void setTileMap(TileMap* tilemap);

    private:
        std::vector<Entity*> obstacles;
        TileMap* tilemap;
    };
}