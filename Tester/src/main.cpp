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

    // Set initial camera rotation
    camera->setRotation(0.0f, 0.0f);

    // Load the model using Model class
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

    // Draw
    prepareRendering();
    drawObjects();
}


void Game::drawObjects()
{
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

    if (!Input::isAnyKeyPressed())
    {
        glm::vec3 pos = player->getTransform().position;
        pos.z -= 2.0f;
        playerLight->setPosition(pos);
        return;
    }

    gllib::Transform transform = player->getTransform();
    glm::vec3 movementDirection = {0.0f, 0.0f, 0.0f};
    bool playerMoved = false;
    int dir = 0;
    if (Input::getKeyPressed(Key_W))
    {
        transform.position.x += 2.0f;
        if (!collisionManager->checkCollision(transform))
        {
            player->move({speed, 0.f, 0.f});
            movementDirection = {1.0f, 0.0f, -0.5f};
            playerMoved = true;
            dir = 0;
        }
    }

    if (Input::getKeyPressed(Key_S))
    {
        transform.position.x -= 2.0f;
        if (!collisionManager->checkCollision(transform))
        {
            player->move({-speed, 0.f, 0.f});
            movementDirection = {-1.0f, 0.0f, -0.5f}; // Moving left with forward bias
            playerMoved = true;
            dir = 1;
        }
    }

    if (Input::getKeyPressed(Key_A))
    {
        transform.position.y -= 2.0f;
        if (!collisionManager->checkCollision(transform))
        {
            player->move({0.f, 0.f, -speed});
            movementDirection = {0.0f, 0.0f, -1.0f}; // Moving forward
            playerMoved = true;
            dir = 2;
        }
    }

    if (Input::getKeyPressed(Key_D))
    {
        transform.position.y += 2.0f;
        if (!collisionManager->checkCollision(transform))
        {
            player->move({0.f, 0, speed});
            movementDirection = {0.0f, 0.0f, 1.0f}; // Moving backward
            playerMoved = true;
            dir = 3;
        }
    }

    glm::vec3 pos = player->getTransform().position;

    switch (dir)
    {
    default:
    case 0: // Moving right
        pos.x += 2.0f;
        break;
    case 1: // Moving left
        pos.x -= 2.0f;
        break;
    case 2: // Moving forward
        pos.y -= 2.0f;
        break;
    case 3: // Moving backward
        pos.y += 2.0f;
        break;
    }

    playerLight->setPosition(pos);

    if (playerMoved)
    {
        glm::vec3 spotDirection = glm::normalize(movementDirection);
        playerLight->setDirection(spotDirection);
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
