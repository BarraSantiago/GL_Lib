#pragma once
#include "Core/base_game.h"
#include <iostream>
#include "Math/collisionManager.h"

class Game : public gllib::BaseGame {
    private:
    gllib::Animation* player;
    gllib::collisionManager* collisionManager;
    gllib::TileMap tileMap;
    float animSpeed, nextFrame;

    void movement(gllib::Animation* player);

    protected:
    void init() override;
    void drawObjects();
    void update() override;
    void uninit() override;

    public:
    Game();
    ~Game() override;
};

