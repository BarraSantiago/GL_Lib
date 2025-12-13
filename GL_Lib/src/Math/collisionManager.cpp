#include "collisionManager.h"

namespace gllib
{
    collisionManager::collisionManager(const std::vector<Entity*>& obstacles) : obstacles{obstacles}
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
        for (Entity* obstacle : obstacles)
        {
            if (obstacle->isColliding(player->getTransform()))
            {
                return true;
            }
        }
        return false;
    }

    bool collisionManager::checkCollision(Transform transform)
    {
        for (Entity* obstacle : obstacles)
        {
            if (obstacle->isColliding(transform))
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
}
