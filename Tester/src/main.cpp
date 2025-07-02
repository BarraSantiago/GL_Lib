#include "base_game.h"

#include <iostream>

#include "collisionManager.h"

using namespace std;

class Game : public gllib::BaseGame
{
private:
    gllib::Cube* player;
    gllib::Rectangle* floorCollision;
    gllib::collisionManager* collisionManager;
    gllib::Cube* cube;
    gllib::AmbientLight* ambientLight;
    gllib::PointLight* pointLight;
    gllib::SpotLight* playerLight;
    gllib::Model* model;
    gllib::Model* model1;
    gllib::Model* model2;
    gllib::Model* model3;
    float modelScale;

    void movement(gllib::Entity* player);

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


    gllib::Transform trs2;
    trs2.position = {0, 0, 0.0f};
    trs2.rotationQuat = {0.0f, 0.0f, 0.0f, 0.0f};
    trs2.scale = {5.0f, 5.0f, 5.0f};
    player = new gllib::Cube(trs2, new gllib::Material(gllib::Material::gold()));

    glm::vec3 playerPos = trs2.position;
    playerPos.z -= 2.0f;
    glm::vec3 spotDirection = {0.0f, 0.0f, -1.0f};
    playerLight = new gllib::SpotLight(playerPos, spotDirection, {1.0f, 1.0f, 0.8f, 1.0f},
                                       5.0f, 55.0f, 1.0f, 0.0045f, 0.00075f);

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

    trs2.position = {-5, 0, 0.0f};

    pointLight = new gllib::PointLight(trs2.position, {1.0f, 1.0f, 1.0f, 1.0f}, 1.0f, 0.0f, 0.0f);
    ambientLight = new gllib::AmbientLight({1.0f, 1.0f, 1.0f}, 0.2f);
    collisionManager = new gllib::collisionManager({static_cast<gllib::Entity*>(floorCollision)});

    model = nullptr;
    modelScale = 5.0f;
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

    camera->setRotation(0.0f, 0.0f);

    try
    {
        model = new gllib::Model("models/claire/source/LXG1NDL0BZ814059Q0RW9HZXE.obj", false);
        std::cout << "Scene model loaded successfully with " << model->meshes.size() << " meshes." << std::endl;
        model1 = new gllib::Model("models/pochita/pochita.fbx", false);
        std::cout << "Scene model loaded successfully with " << model1->meshes.size() << " meshes." << std::endl;
        model2 = new gllib::Model("models/chicken/Chicken1.fbx", false);
        std::cout << "Scene model loaded successfully with " << model2->meshes.size() << " meshes." << std::endl;
        model3 = new gllib::Model("models/Backpack/backpack.mtl", false);
        std::cout << "Scene model loaded successfully with " << model3->meshes.size() << " meshes." << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << "Failed to load scene model: " << e.what() << std::endl;
        model = nullptr;
    }
    model->transform.scale *= 10;
    model->transform.position = {10.0f, 0.0f, 10.0f};
    model->transform.rotationQuat = {0.0f, 0.0f, 0.30f, 0.0f};
    model1->transform = model->transform;
    model1->transform.position.z += 10.0f;
    model2->transform = model->transform;
    model2->transform.position.x += 10.0f;
    model3->transform = model->transform;
    model3->transform.position.z -= 10.0f;

    srand(time(nullptr));
    window->setTitle("Engine");
}


void Game::update()
{
    // Update
    movement(player);
    if (cameraController->getCameraMode() == gllib::CameraMode::ThirdPerson)
    {
        camera->updateThirdPersonPosition();
    }

    float rotationSpeed = 30.0f * gllib::LibTime::getDeltaTime();
    float angleInRadians = glm::radians(rotationSpeed);

    gllib::Quaternion rotationZ;
    rotationZ.w = std::cos(angleInRadians / 2.0f);
    rotationZ.x = 0.0f;
    rotationZ.z = 0.0f;
    rotationZ.y = std::sin(angleInRadians / 2.0f);

    gllib::Quaternion cubeRot = cube->getRotationQuat();

    gllib::Quaternion newRot;
    newRot.w = rotationZ.w * cubeRot.w - rotationZ.x * cubeRot.x - rotationZ.y * cubeRot.y - rotationZ.z * cubeRot.z;
    newRot.x = rotationZ.w * cubeRot.x + rotationZ.x * cubeRot.w + rotationZ.y * cubeRot.z - rotationZ.z * cubeRot.y;
    newRot.y = rotationZ.w * cubeRot.y - rotationZ.x * cubeRot.z + rotationZ.y * cubeRot.w + rotationZ.z * cubeRot.x;
    newRot.z = rotationZ.w * cubeRot.z + rotationZ.x * cubeRot.y - rotationZ.y * cubeRot.x + rotationZ.z * cubeRot.w;

    newRot.normalize();

    cube->setRotationQuat(newRot);

    // Draw
    drawObjects();
}


void Game::drawObjects()
{
    gllib::Renderer::clear();

    gllib::Shader::setShaderProgram(shaderProgramLighting);

    ambientLight->apply(shaderProgramLighting);
    pointLight->apply(shaderProgramLighting);
    playerLight->apply(shaderProgramLighting);

    // Draw the loaded scene model
    if (model != nullptr)
    {
        model->draw();
    }

    model1->draw();
    model2->draw();
    model3->draw();

    cube->draw();
    player->draw();

    gllib::Shader::setShaderProgram(shaderProgramTexture);

    gllib::Shader::setShaderProgram(shaderProgramSolidColor);
}


void Game::movement(gllib::Entity* player)
{
    gllib::Transform transform2 = player->getTransform();
    transform2.position.y += 1.f;
    float speed = 80 * gllib::LibTime::getDeltaTime();

    if (!Input::isAnyKeyPressed())
    {
        glm::vec3 pos = player->getTransform().position;
        pos.z -= 2.0f;
        playerLight->setPosition(pos);
        return;
    }

    // Get camera orientation vectors
    glm::vec3 cameraFront = camera->getFront();
    glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, glm::vec3(0.0f, 1.0f, 0.0f)));

    cameraFront.y = 0.0f;
    cameraFront = glm::normalize(cameraFront);
    cameraRight.y = 0.0f;
    cameraRight = glm::normalize(cameraRight);

    gllib::Transform transform = player->getTransform();
    glm::vec3 movementDirection = {0.0f, 0.0f, 0.0f};
    glm::vec3 moveVector = {0.0f, 0.0f, 0.0f};
    bool playerMoved = false;

    if (Input::getKeyPressed(Key_W))
    {
        moveVector = cameraFront * speed;
        transform.position += moveVector;
        if (!collisionManager->checkCollision(transform))
        {
            player->move(moveVector);
            movementDirection = cameraFront;
            playerMoved = true;
        }
    }

    if (Input::getKeyPressed(Key_S))
    {
        moveVector = -cameraFront * speed;
        transform.position += moveVector;
        if (!collisionManager->checkCollision(transform))
        {
            player->move(moveVector);
            movementDirection = -cameraFront;
            playerMoved = true;
        }
    }

    if (Input::getKeyPressed(Key_A))
    {
        moveVector = -cameraRight * speed;
        transform.position += moveVector;
        if (!collisionManager->checkCollision(transform))
        {
            player->move(moveVector);
            movementDirection = -cameraRight;
            playerMoved = true;
        }
    }

    if (Input::getKeyPressed(Key_D))
    {
        moveVector = cameraRight * speed;
        transform.position += moveVector;
        if (!collisionManager->checkCollision(transform))
        {
            player->move(moveVector);
            movementDirection = cameraRight;
            playerMoved = true;
        }
    }

    glm::vec3 pos = player->getTransform().position;
    if (playerMoved)
    {
        pos += glm::normalize(movementDirection) * 2.0f;
        playerLight->setPosition(pos);

        glm::vec3 spotDirection = glm::normalize(movementDirection);
        spotDirection.y = -0.5f; 
        playerLight->setDirection(glm::normalize(spotDirection));
    }
    else
    {
        pos += cameraFront * 2.0f;
        playerLight->setPosition(pos);
    }
}

void Game::uninit()
{
    cout << "External uninit!!!\n";
    delete ambientLight;
    delete cube;
    delete player;
    delete model;
    delete playerLight;
}

int main()
{
    Game game;
    game.start();
}
