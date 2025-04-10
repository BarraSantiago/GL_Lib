#include "base_game.h"

#include <iostream>

#include "collisionManager.h"

using namespace std;

class Game : public gllib::BaseGame {
private:
    gllib::Triangle* triangle;
    gllib::Sprite* sprite;
    gllib::Sprite* background;
    gllib::Animation* coin;
    gllib::Animation* player;
    gllib::Rectangle* floorCollision;
    gllib::collisionManager* collisionManager;
    float animSpeed, nextFrame;

    void moveRectangle(float speed);
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

Game::Game()
{
    window->setVsyncEnabled(false);
    cout << "Game created!\n";

    gllib::Transform trs;
    trs.position = { 100.0f, 100.0f, -2.0f };
    trs.rotationQuat = { 0.0f, 0.0f, 0.0f, 1.0f };
    trs.scale = { 57.74f, 50.0f, 0.0f };
    triangle = new gllib::Triangle(trs, { 0.85f, 0.2f, 0.4f, 1.0f });
    
    gllib::Transform trs2;
    trs2.position = { 400.0f, 400.0f, 1.0f };
    trs2.rotationQuat = { 0.0f, 0.0f, 0.0f, 90.0f };
    trs2.scale = { 100.0f, 100.0f, 0.0f };
    sprite = new gllib::Sprite(trs2, { 1.0f, 1.0f, 1.0f, 1.0f });
    trs2.position = { 400.0f, 400.0f, 2.0f };
    coin = new gllib::Animation(trs2, { 1.0f, 1.0f, 1.0f, 1.0f });
    trs2.position = { window->getWidth() * .5f, window->getHeight() * .5f, 0.0f };
    player = new gllib::Animation(trs2, { 1.0f, 1.0f, 1.0f, 1.0f });

    gllib::Transform trs3;
    trs3.position = { window->getWidth() * .5f, window->getHeight() * .5f, 0.0f };
    trs3.rotationQuat = { 0.0f, 0.0f, 0.0f, 0.0f };
    trs3.scale = { 640.0f, 480.0f, 0.0f };
    background = new gllib::Sprite(trs3, { 1.0f, 1.0f, 1.0f, 1.0f });
    background->addTexture("background.png", true);

    gllib::Transform trs4;
    trs4.position = {window->getWidth() * .5f, window->getHeight() * .95f, 3};
    trs4.rotationQuat = { 0.0f, 0.0f, 0.0f, 0.0f };
    trs4.scale = {static_cast<float>(window->getWidth()), 80, 0};
    floorCollision = new gllib::Rectangle(trs4, {0.8f, 0.0f, 1.0f, 0.5f});

    collisionManager = new gllib::collisionManager({static_cast<gllib::Entity*>(floorCollision)});
    
    sprite->addTexture("sus.png", true);
    sprite->setMirroredX(true);
    int textureWidth = 16;
    unsigned int coinTex = gllib::Loader::loadTexture("coin.png", true);
    coin->addFrames(coinTex, textureWidth, 16, 8, 1);
    coin->setCurrentFrame(7);

    unsigned int sonicTex = gllib::Loader::loadTexture("Sonic_Atlas.png", true);
    player->addFrame(sonicTex, 273, 118, 33, 41);
    player->addFrame(sonicTex, 305, 118, 33, 41);
    player->addFrame(sonicTex, 340, 118, 35, 41);
    player->addFrame(sonicTex, 379, 118, 38, 41);
    player->addFrame(sonicTex, 418, 118, 36, 41);
    player->addFrame(sonicTex, 456, 118, 32, 41);
    player->addFrame(sonicTex, 488, 118, 32, 41);
    player->addFrame(sonicTex, 522, 118, 32, 41);
    player->addFrame(sonicTex, 558, 118, 34, 41);
    player->addFrame(sonicTex, 595, 118, 37, 41);
    player->addFrame(sonicTex, 636, 118, 34, 41);
    player->addFrame(sonicTex, 673, 118, 32, 41);

    //player->addFramesFromAtlas(sonicTex, 277, 118, 35, 40, 12, 1);

    player->setCurrentFrame(0);
    player->setDurationInSecs(1.f);
    
    coin->setCurrentFrame(0);

    coin->setDurationInSecs(.6);

    animSpeed = .075f;
    nextFrame = 0;
}

Game::~Game() {
    cout << "Game destroyed!\n";
}

void Game::init() {
    cout << "External init!\n";

    camera->setTarget(player);
    camera->setDistance(100.0f); // Distance behind player
    camera->setHeight(2.0f);   // Height above player
    
    // Set initial camera rotation
    camera->setRotation(0.0f, 0.0f);

    srand(time(nullptr));
    window->setTitle("Engine");
}


void Game::update() {
    // Update
    movement(player);
    camera->updateThirdPersonPosition();

    coin->update();
    player->update();

    if (triangle != nullptr) {
        gllib::Quaternion rot = triangle->getRotationQuat();
        rot.z += gllib::LibTime::getDeltaTime() * 30.0f;
        triangle->setRotationQuat(rot);
    }

    moveRectangle(100);

    // Draw
    
    
    drawObjects();
}


void Game::drawObjects()
{
    gllib::Renderer::clear();

    gllib::Shader::useShaderProgram(shaderProgramTexture);
    background->draw();
    sprite->draw();
    coin->draw();
    player->draw();

    gllib::Shader::useShaderProgram(shaderProgramSolidColor);
    triangle->draw();
}

static int x = 1;
static int y = 1;

void Game::moveRectangle(float speed) {
    if (sprite->getPosition().x - (sprite->getScale().x * .5) <= 0) {
        x = 1;
        sprite->setMirroredX(true);
    }
    if (sprite->getPosition().x + (sprite->getScale().x * .5) >= window->getWidth()) {
        x = -1;
        sprite->setMirroredX(false);
        gllib::Vector3 scale = coin->getScale();
    }

    if (sprite->getPosition().y - (sprite->getScale().y * .5) <= 0) {
        y = 1;
        sprite->setMirroredY(true);
    }
    if (sprite->getPosition().y + (sprite->getScale().y * .5) >= window->getHeight()) {
        y = -1;
        sprite->setMirroredY(false);
    }
    
    gllib::Vector3 scale = coin->getScale();
    scale.x += (25.0f * gllib::LibTime::getDeltaTime()) * x;
    scale.y += (25.0f * gllib::LibTime::getDeltaTime()) * x;
    coin->setScale(scale);

    sprite->move({static_cast<float>(x * (speed * gllib::LibTime::getDeltaTime())), 
                  static_cast<float>(y * (speed * gllib::LibTime::getDeltaTime())), 
                  0.0f});
    //sprite->rotate({ 0.0f, 0.0f, static_cast<float>(gllib::LibTime::getDeltaTime() * -60.0f) });
}

void Game::movement(gllib::Animation* player)
{
    gllib::Transform transform2 = player->getTransform();
    transform2.position.y += 1.f;
    float speed = 80 * gllib::LibTime::getDeltaTime();
    float gravity = 40 * gllib::LibTime::getDeltaTime();
    if (!collisionManager->checkCollision(transform2))
    {
        player->move({0.f, gravity, 0});
    }

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
        player->setMirroredX(false);
        if (!collisionManager->checkCollision(transform))
        {
            player->move({speed, 0.f, 0.f});
        }
    }

    if (Input::getKeyPressed(Key_A))
    {
        // A
        transform.position.x -= 2.0f;
        player->setMirroredX(true);
        if (!collisionManager->checkCollision(transform))
        {
            player->move({-speed, 0.f, 0.f});
        }
    }
    
    if (Input::getKeyPressed(Key_W))
    {
        // W
        transform.position.y -= 2.0f;
        if (!collisionManager->checkCollision(transform))
        {
            player->move({0.f, -speed, 0.f});
        }
    }

    if (Input::getKeyPressed(Key_S))
    {
        // S
        transform.position.y += 2.0f;
        if (!collisionManager->checkCollision(transform))
        {
            player->move({0.f, speed, 0.f});
        }
    }
}

void Game::uninit() {
    cout << "External uninit!!!\n";
    delete triangle;
    delete sprite;
    delete coin;
}

int main() {
    Game game;
    game.start();
}