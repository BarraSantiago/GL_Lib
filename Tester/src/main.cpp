#include "base_game.h"

#include <iostream>

#include "collisionManager.h"

using namespace std;

class Game : public gllib::BaseGame
{
private:
    gllib::Triangle* triangle;
    gllib::Sprite* sprite;
    gllib::Sprite* background;
    gllib::Animation* coin;
    gllib::Cube* player;
    gllib::Rectangle* floorCollision;
    gllib::collisionManager* collisionManager;
    gllib::Cube* cube;
    gllib::AmbientLight* ambientLight;
    gllib::PointLight* pointLight;
    glm::vec3 modelPosition;
    float modelScale;
    float animSpeed, nextFrame;

    void moveRectangle(float speed);
    void movement(gllib::Entity* player);

protected:
    void init() override;
    void drawObjects();
    void prepareRendering();
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
    trs.position = {100.0f, 100.0f, -2.0f};
    trs.rotationQuat = {0.0f, 0.0f, 0.0f, 1.0f};
    trs.scale = {57.74f, 50.0f, 0.0f};
    triangle = new gllib::Triangle(trs, {0.85f, 0.2f, 0.4f, 1.0f});

    gllib::Transform trs2;
    trs2.position = {400.0f, 400.0f, 1.0f};
    trs2.rotationQuat = {0.0f, 0.0f, 0.0f, 90.0f};
    trs2.scale = {100.0f, 100.0f, 0.0f};
    sprite = new gllib::Sprite(trs2, {1.0f, 1.0f, 1.0f, 1.0f});

    trs2.position = {400.0f, 400.0f, 2.0f};
    coin = new gllib::Animation(trs2, {1.0f, 1.0f, 1.0f, 1.0f});

    trs2.position = {0, 0, 0.0f};
    trs2.rotationQuat = {0.0f, 0.0f, 0.0f, 0.0f};
    trs2.scale = {5.0f, 5.0f, 5.0f};
    player = new gllib::Cube(trs2, new gllib::Material(gllib::Material::emerald()));

    gllib::Transform trs3;
    trs3.position = {window->getWidth() * .5f, window->getHeight() * .5f, -50.0f};
    trs3.rotationQuat = {0.0f, 0.0f, 0.0f, 0.0f};
    trs3.scale = {640.0f, 480.0f, 0.0f};
    background = new gllib::Sprite(trs3, {1.0f, 1.0f, 1.0f, 1.0f});

    gllib::Transform trs4;
    trs4.position = {window->getWidth() * .5f, window->getHeight() * .95f, 3};
    trs4.rotationQuat = {0.0f, 0.0f, 0.0f, 0.0f};
    trs4.scale = {static_cast<float>(window->getWidth()), 80, 0};
    floorCollision = new gllib::Rectangle(trs4, {0.8f, 0.0f, 1.0f, 0.5f});

    gllib::Transform cubeTrs;
    cubeTrs.position = {0.0f, 0.0f, -10.0f};
    cubeTrs.rotationQuat = {10.0f, 10.0f, 10.0f, 10.0f};
    cubeTrs.scale = {10.0f, 10.0f, 10.0f};
    cube = new gllib::Cube(cubeTrs, new gllib::Material(gllib::Material::bronze()));


    pointLight = new gllib::PointLight(trs2.position, {1.0f, 1.0f, 1.0f}, 1.0f, 0.09f, 0.012f);
    ambientLight = new gllib::AmbientLight({1.0f, 1.0f, 1.0f, 1.0f}, 0.8f);

    collisionManager = new gllib::collisionManager({static_cast<gllib::Entity*>(floorCollision)});

    sprite->addTexture("sus.png", true);
    sprite->setMirroredX(true);
    int textureWidth = 16;
    unsigned int coinTex = gllib::Loader::loadTexture("coin.png", true);
    coin->addFrames(coinTex, textureWidth, 16, 8, 1);
    coin->setCurrentFrame(7);

    coin->setCurrentFrame(0);

    coin->setDurationInSecs(.6);
    modelPosition = {0.0f, 0.0f, -5.0f};
    modelScale = 10.0f;
    animSpeed = .075f;
    nextFrame = 0;
}

Game::~Game()
{
    cout << "Game destroyed!\n";
}

void Game::init()
{
    cout << "External init!\n";

    camera->setTarget(player);
    camera->setDistance(100.0f);
    camera->setHeight(2.0f);
    camera->setPerspective(45.0f, window->getWidth() / (float)window->getHeight(), 0.1f, 1000.0f);
    camera->invertYAxis();

    // Set initial camera rotation
    camera->setRotation(0.0f, 0.0f);
    if (!importer->loadModel("models/kitty/FroggieKitty_11.fbx  "))
    {
        std::cout << "Failed to load model!" << std::endl;
    }
    else
    {
        std::cout << "Model loaded successfully with " << importer->getMeshCount() << " meshes." << std::endl;
    }
    srand(time(nullptr));
    window->setTitle("Engine");
}


void Game::update()
{
    
    // Update
    movement(player);
    camera->updateThirdPersonPosition();

    coin->update();

    // In your update() function, replace the current quaternion code with:
    float rotationSpeed = 30.0f * gllib::LibTime::getDeltaTime();
    float angleInRadians = glm::radians(rotationSpeed);

    // Create a rotation quaternion for the z-axis
    gllib::Quaternion rotationZ;
    rotationZ.w = std::cos(angleInRadians / 2.0f);
    rotationZ.x = 0.0f;
    rotationZ.z = 0.0f;
    rotationZ.y = std::sin(angleInRadians / 2.0f);

    // Get the current rotation
    gllib::Quaternion cubeRot = cube->getRotationQuat();

    // Multiply the quaternions (the order matters here)
    gllib::Quaternion newRot;
    newRot.w = rotationZ.w * cubeRot.w - rotationZ.x * cubeRot.x - rotationZ.y * cubeRot.y - rotationZ.z * cubeRot.z;
    newRot.x = rotationZ.w * cubeRot.x + rotationZ.x * cubeRot.w + rotationZ.y * cubeRot.z - rotationZ.z * cubeRot.y;
    newRot.y = rotationZ.w * cubeRot.y - rotationZ.x * cubeRot.z + rotationZ.y * cubeRot.w + rotationZ.z * cubeRot.x;
    newRot.z = rotationZ.w * cubeRot.z + rotationZ.x * cubeRot.y - rotationZ.y * cubeRot.x + rotationZ.z * cubeRot.w;

    // Normalize to ensure it's a pure rotation
    newRot.normalize();

    // Apply the new rotation
    cube->setRotationQuat(newRot);

    if (triangle != nullptr)
    {
        gllib::Quaternion rot = triangle->getRotationQuat();
        rot.z += gllib::LibTime::getDeltaTime() * 30.0f;
        triangle->setRotationQuat(rot);
    }

    moveRectangle(100);

    // Draw
    prepareRendering();
    drawObjects();
}


void Game::drawObjects()
{
    gllib::Shader::setShaderProgram(shaderProgramLighting);

    ambientLight->apply(shaderProgramLighting);
    pointLight->apply(shaderProgramLighting);
    
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, modelPosition);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(modelScale));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    
    gllib::Shader::setMat4(shaderProgramLighting, "model", modelMatrix);
    
    for (size_t i = 0; i < importer->getMeshCount(); i++)
    {
        gllib::RenderData renderData = importer->getRenderData(i);
        size_t indicesCount = importer->getIndicesCount(i);

        if (importer->hasTexture(i))
        {
            gllib::Renderer::drawTexture(renderData, static_cast<GLsizei>(indicesCount),
                                         importer->getTexture(i));
        }
        else
        {
            gllib::Renderer::drawElements(renderData, static_cast<GLsizei>(indicesCount));
        }
    }

    cube->draw();
    player->draw();

    gllib::Shader::setShaderProgram(shaderProgramTexture);
    //background->draw();
    //sprite->draw();
    //coin->draw();

    gllib::Shader::setShaderProgram(shaderProgramSolidColor);
    triangle->draw();
}

void Game::prepareRendering()
{
    gllib::Renderer::clear();

    // Set up lighting shader
    gllib::Shader::setShaderProgram(shaderProgramLighting);
    glm::mat4 projection = camera->getProjectionMatrix();
    glm::mat4 view = camera->getViewMatrix();

    gllib::Shader::setMat4(shaderProgramLighting, "projection", projection);
    gllib::Shader::setMat4(shaderProgramLighting, "view", view);
    gllib::Shader::setVec3(shaderProgramLighting, "viewPos",
                           camera->getPosition().x,
                           camera->getPosition().y,
                           camera->getPosition().z);

    // Set up texture shader
    gllib::Shader::setShaderProgram(shaderProgramTexture);
    gllib::Shader::setMat4(shaderProgramTexture, "projection", projection);
    gllib::Shader::setMat4(shaderProgramTexture, "view", view);

    // Set up solid color shader
    gllib::Shader::setShaderProgram(shaderProgramSolidColor);
    gllib::Shader::setMat4(shaderProgramSolidColor, "projection", projection);
    gllib::Shader::setMat4(shaderProgramSolidColor, "view", view);
}

static int x = 1;
static int y = 1;

void Game::moveRectangle(float speed)
{
    if (sprite->getPosition().x - (sprite->getScale().x * .5) <= 0)
    {
        x = 1;
        sprite->setMirroredX(true);
    }
    if (sprite->getPosition().x + (sprite->getScale().x * .5) >= window->getWidth())
    {
        x = -1;
        sprite->setMirroredX(false);
        gllib::Vector3 scale = coin->getScale();
    }

    if (sprite->getPosition().y - (sprite->getScale().y * .5) <= 0)
    {
        y = 1;
        sprite->setMirroredY(true);
    }
    if (sprite->getPosition().y + (sprite->getScale().y * .5) >= window->getHeight())
    {
        y = -1;
        sprite->setMirroredY(false);
    }

    gllib::Vector3 scale = coin->getScale();
    scale.x += (25.0f * gllib::LibTime::getDeltaTime()) * x;
    scale.y += (25.0f * gllib::LibTime::getDeltaTime()) * x;
    coin->setScale(scale);

    sprite->move({
        static_cast<float>(x * (speed * gllib::LibTime::getDeltaTime())),
        static_cast<float>(y * (speed * gllib::LibTime::getDeltaTime())),
        0.0f
    });
    //sprite->rotate({ 0.0f, 0.0f, static_cast<float>(gllib::LibTime::getDeltaTime() * -60.0f) });
}

void Game::movement(gllib::Entity* player)
{
    gllib::Transform transform2 = player->getTransform();
    transform2.position.y += 1.f;
    float speed = 80 * gllib::LibTime::getDeltaTime();
    float gravity = 40 * gllib::LibTime::getDeltaTime();
    if (!collisionManager->checkCollision(transform2))
    {
        //player->move({0.f, gravity, 0});
    }

    if (!Input::isAnyKeyPressed()) return;
    gllib::Transform transform = player->getTransform();

    if (Input::getKeyPressed(Key_D))
    {
        // D
        transform.position.x += 2.0f;
        if (!collisionManager->checkCollision(transform))
        {
            player->move({speed, 0.f, 0.f});
        }
    }

    if (Input::getKeyPressed(Key_A))
    {
        // A
        transform.position.x -= 2.0f;
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

void Game::uninit()
{
    cout << "External uninit!!!\n";
    delete ambientLight;
    delete cube;
    delete triangle;
    delete sprite;
    delete coin;
    delete player;
}

int main()
{
    Game game;
    game.start();
}
