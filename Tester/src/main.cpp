#include "Core/base_game.h"

#include <iostream>

#include "Math/collisionManager.h"

using namespace std;

//TODO SEPARATE IN CLASSES, CLEAN UP
class Game : public gllib::BaseGame {
private:
    gllib::Animation* player;
    gllib::Rectangle* floorCollision;
    gllib::collisionManager* collisionManager;
    gllib::TileMap tileMap;
    float animSpeed, nextFrame;

    void movement(gllib::Animation* player);

protected:
    void init() override;
    void drawObjects();
    void update() override;
    void uninit() override;

    unsigned int tileMapTex;
public:
    Game();
    ~Game() override;
};

Game::Game()
{
    window->setVsyncEnabled(false);
    cout << "Game created!\n";

    tileMapTex = gllib::Loader::loadTexture("free_pixel_16_woods.png", true);
    unsigned int battleCityTex = gllib::Loader::loadTexture("Battle City Atlas.png", true);
    unsigned int baseTex = gllib::Loader::loadTexture("free_pixel_16_woods.png", true);
    tileMap = gllib::TileMap::LoadFromTiledXML("baseMap2.tmx", baseTex);
    
    gllib::Transform trs2;
    trs2.position = { 400.0f, 400.0f, 5.0f };
    trs2.rotationQuat = { 0.0f, 0.0f, 0.0f, 0.0f };
    trs2.position = { window->getWidth() * .5f, window->getHeight() * .5f, 2.5f };
    trs2.scale = { 25.0f, 25.0f, 1.0f };
    player = new gllib::Animation(trs2, { 1.0f, 1.0f, 1.0f, 1.0f });
    
    gllib::Transform trs4;
    trs4.position = {window->getWidth() * .5f, window->getHeight() * .95f, 0};
    trs4.rotationQuat = { 0.0f, 0.0f, 0.0f, 0.0f };
    trs4.scale = {static_cast<float>(window->getWidth()), 80, 0};
    floorCollision = new gllib::Rectangle(trs4, {0.8f, 0.0f, 1.0f, 0.5f});

    collisionManager = new gllib::collisionManager({static_cast<gllib::Entity*>(floorCollision)}, &tileMap);
    
    player->addFrames(battleCityTex, 16,16, 2,1,0,0);
    player->setCurrentAnimation(1);
    player->addFrames(battleCityTex, 16,16, 4,1,2,0);

    player->setCurrentFrame(0);
    player->setDurationInSecs(1.f);

    animSpeed = .075f;
    nextFrame = 0;
}

Game::~Game() {
    cout << "Game destroyed\n";
}

void Game::init() {
    cout << "External init\n";

    srand(time(nullptr));
    window->setTitle("Tilemap");
}


void Game::update() {
    // Update
    movement(player);

    player->update();
    
    // Draw
    drawObjects();
}


void Game::drawObjects()
{
    gllib::Renderer::clear();

    gllib::Shader::useShaderProgram(shaderProgramTexture);
   
    tileMap.draw();
    player->draw();

    gllib::Shader::useShaderProgram(shaderProgramSolidColor);
}

// TODO MOVE THE ACTUAL PLAYER, NOT USE A TRANSFORM COPY
void Game::movement(gllib::Animation* player)
{
    float speed = 80 * gllib::LibTime::getDeltaTime();

    if (Input::getKeyReleased(Key_R)) {
        player->setAnimationPaused(true);
        player->reset();
    }

    if (Input::getKeyPressed(Key_Q)) {
        player->setAnimationPaused(false);
    }

    if (!Input::isAnyKeyPressed()) return;
    gllib::Transform transform = player->getTransform();

    if (Input::getKeyPressed(Key_D))
    {
        // D
        transform.position.x += 2.0f;
        player->setMirroredX(true);
        player->setCurrentAnimation(1);
        player->setRotationEuler({0,180,0});
        if (!collisionManager->checkCollision(transform))
        {
            player->move({speed, 0.f, 0.f});
        }
    }

    if (Input::getKeyPressed(Key_A))
    {
        // A
        transform.position.x -= 2.0f;
        player->setCurrentAnimation(1);
        player->setMirroredX(false);
        if (!collisionManager->checkCollision(transform))
        {
            player->move({-speed, 0.f, 0.f});
        }
    }
    
    if (Input::getKeyPressed(Key_W))
    {
        // W
        transform.position.y -= 2.0f;
        player->setCurrentAnimation(0);
        player->setMirroredY(false);
        if (!collisionManager->checkCollision(transform))
        {
            player->move({0.f, -speed, 0.f});
        }
    }

    if (Input::getKeyPressed(Key_S))
    {
        // S
        player->setMirroredY(true);
        player->setCurrentAnimation(0);
        transform.position.y += 2.0f;
        if (!collisionManager->checkCollision(transform))
        {
            player->move({0.f, speed, 0.f});
        }
    }
}

void Game::uninit() {
    cout << "External uninit!!!\n";
    delete player;
}

int main() {
    Game game;
    game.start();
}