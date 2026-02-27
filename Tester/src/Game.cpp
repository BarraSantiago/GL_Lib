#include "Game.h"
using namespace std;

//TODO SEPARATE IN CLASSES, CLEAN UP

Game::Game()
{
    window->setVsyncEnabled(false);
    cout << "Game created!\n";

    unsigned int battleCityTex = gllib::Loader::loadTexture("Battle City Atlas.png", true);
    tileMap = gllib::TileMap::LoadFromTiledXML("BattleCity.tmx", battleCityTex);

    gllib::Transform trs2;
    trs2.rotationQuat = {0.0f, 0.0f, 0.0f, 0.0f};
    trs2.position = {window->getWidth() * .5f, window->getHeight() * .5f, 2.5f};
    trs2.scale = {32.0f, 32.0f, 1.0f};
    player = new gllib::Animation(trs2, {1.0f, 1.0f, 1.0f, 1.0f});
    
    collisionManager = new gllib::collisionManager({}, &tileMap);

    player->addFrames(battleCityTex, 16, 16, 2, 1, 0, 0);
    player->setCurrentAnimation(1);
    player->addFrames(battleCityTex, 16, 16, 4, 1, 2, 0);

    player->setCurrentFrame(0);
    player->setDurationInSecs(1.f);

    animSpeed = .075f;
    nextFrame = 0;
}

Game::~Game()
{
    cout << "Game destroyed\n";
}

void Game::init()
{
    cout << "External init\n";

    srand(time(nullptr));
    window->setTitle("Tilemap");
}


void Game::update()
{
    // Update
    movement(player);

    player->update();

    // Draw
    drawObjects();
}


void Game::drawObjects()
{
    gllib::Renderer::clear();

    gllib::Shader::useShaderProgram(gllib::Shader::textureShaderProgram);

    tileMap.draw();
    player->draw();

    gllib::Shader::useShaderProgram(gllib::Shader::shapeShaderProgram);
}

// TODO MOVE THE ACTUAL PLAYER, NOT USE A TRANSFORM COPY

void Game::movement(gllib::Animation* player)
{
    float speed = 80 * gllib::LibTime::getDeltaTime();

    if (Input::getKeyReleased(Key_R))
    {
        player->setAnimationPaused(true);
        player->reset();
    }

    if (Input::getKeyPressed(Key_Q))
    {
        player->setAnimationPaused(false);
    }

    if (!Input::isAnyKeyPressed()) return;

    if (Input::getKeyPressed(Key_D))
    {
        // D
        player->move({speed, 0.f, 0.f});
        player->setMirroredX(true);
        player->setCurrentAnimation(1);
        player->setRotationEuler({0, 180, 0});
        if (collisionManager->checkCollision(player->getTransform()))
        {
            player->move({- speed, 0.f, 0.f});
        }
    }

    if (Input::getKeyPressed(Key_A))
    {
        // A
        player->move({-speed, 0.f, 0.f});
        player->setCurrentAnimation(1);
        player->setMirroredX(false);
        if (collisionManager->checkCollision(player->getTransform()))
        {
            player->move({speed, 0.f, 0.f});
        }
    }

    if (Input::getKeyPressed(Key_W))
    {
        // W
        player->move({0.f, -speed, 0.f});
        player->setCurrentAnimation(0);
        player->setMirroredY(false);
        if (collisionManager->checkCollision(player->getTransform()))
        {
            player->move({0.f, speed, 0.f});
        }
    }

    if (Input::getKeyPressed(Key_S))
    {
        // S
        player->move({0.f, speed, 0.f});
        player->setMirroredY(true);
        player->setCurrentAnimation(0);
        if (collisionManager->checkCollision(player->getTransform()))
        {
            player->move({0.f, -speed, 0.f});
        }
    }
}

void Game::uninit()
{
    cout << "External uninit!!!\n";
    delete player;
}
