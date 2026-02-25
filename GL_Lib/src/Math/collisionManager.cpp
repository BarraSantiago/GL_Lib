#include "collisionManager.h"

#include "myMaths.h"

namespace gllib
{
    collisionManager::collisionManager(const std::vector<Entity*>& obstacles, TileMap* tilemap)
        : obstacles{obstacles}, tilemap{tilemap}
    {
    }

    collisionManager::~collisionManager()
    {
        for (Entity* obstacle : obstacles)
        {
            delete obstacle;
        }

        obstacles.clear();
    }

    bool collisionManager::checkCollision(Entity* entity)
    {
        return checkCollision(entity->getTransform());
    }

    bool collisionManager::checkCollision(Transform transform)
    {
        // Check entity obstacles
        for (Entity* obstacle : obstacles)
        {
            if (Maths::checkAABB(obstacle->getTransform(), transform))
            {
                return true;
            }
        }

        return checkTileMapCollision(transform);
    }
    
    bool collisionManager::checkTileMapCollision(const Transform& transform) const
    {
        if (tilemap == nullptr) return false;

        int minCol = static_cast<int>(std::floor(transform.position.x / tilemap->tileWidth));
        int minRow = static_cast<int>(std::floor(transform.position.y / tilemap->tileHeight));
        int maxCol = static_cast<int>(std::floor((transform.position.x + transform.scale.x - 0.001f) / tilemap->tileWidth));
        int maxRow = static_cast<int>(std::floor((transform.position.y + transform.scale.y - 0.001f) / tilemap->tileHeight));

        minCol = std::max(0, minCol);
        minRow = std::max(0, minRow);
        maxCol = std::min(tilemap->width - 1, maxCol);
        maxRow = std::min(tilemap->height - 1, maxRow);

        if (minCol > maxCol || minRow > maxRow) return false;

        for (const TileLayer& layer : tilemap->layers)
        {
            for (int row = minRow; row <= maxRow; ++row)
            {
                for (int col = minCol; col <= maxCol; ++col)
                {
                    const Tile& t = layer.tiles[row][col];
                    if (t.empty() || t.walkable) continue;

                    if (Maths::checkAABB(t.getTransform(), transform))
                        return true;
                }
            }
        }

        return false;
    }
    
    void collisionManager::addObstacle(Entity* obstacle)
    {
        obstacles.push_back(obstacle);
    }

    void collisionManager::removeObstacle(Entity* obstacle_)
    {
        for (int i = 0; i < obstacles.size(); i++)
        {
            if (obstacles[i] == obstacle_)
            {
                obstacles.erase(obstacles.begin() + i);
                break;
            }
        }
    }

    void collisionManager::setTileMap(TileMap* tilemap)
    {
        this->tilemap = tilemap;
    }
}
