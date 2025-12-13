#include "collisionManager.h"

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

    bool collisionManager::checkCollision(Entity* player)
    {
        Transform t = player->getTransform();
        
        // Check entity obstacles
        for (Entity* obstacle : obstacles)
        {
            if (obstacle->isColliding(t))
            {
                return true;
            }
        }

        // Check tilemap collision
        if (tilemap != nullptr)
        {
            Vector3 pos = t.position;
            Vector3 scale = t.scale;
            
            if (tilemap->checkCollisionAABB(pos.x, pos.y, scale.x, scale.y))
            {
                return true;
            }
        }

        return false;
    }

    bool collisionManager::checkCollision(Transform transform)
    {
        // Check entity obstacles
        for (Entity* obstacle : obstacles)
        {
            if (obstacle->isColliding(transform))
            {
                return true;
            }
        }

        // Check tilemap collision
        if (tilemap != nullptr)
        {
            Vector3 pos = transform.position;
            Vector3 scale = transform.scale;
            
            if (tilemap->checkCollisionAABB(pos.x, pos.y, scale.x, scale.y))
            {
                return true;
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