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

        float left = transform.position.x - 0.5f * transform.scale.x;
        float top = transform.position.y - 0.5f * transform.scale.y;
        float right = transform.position.x + 0.5f * transform.scale.x;
        float bottom = transform.position.y + 0.5f * transform.scale.y;

        int minCol = static_cast<int>(std::floor(left / tilemap->tileWidth) - 2);
        int minRow = static_cast<int>(std::floor(top / tilemap->tileHeight) - 2);
        int maxCol = static_cast<int>(std::ceil(right / tilemap->tileWidth)) + 2;
        int maxRow = static_cast<int>(std::ceil(bottom / tilemap->tileHeight)) + 2;

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
