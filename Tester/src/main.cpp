#include "Core/base_game.h"

#include <iostream>

#include "Math/transform.h"
#include "Rendering/renderer.h"


using namespace std;
using namespace gllib;

class Game : public BaseGame
{
private:
    Cube* player;
    Rectangle* floorCollision;
    collisionManager* collisionManager;
    Cube* cube;
    AmbientLight* ambientLight;
    PointLight* pointLight;
    SpotLight* playerLight;
    Model* model;
    Model* model1;
    Model* model2;
    Model* model3;
    float modelScale;
    bool hierarchyTestMode = false;
    float testTimer = 0.0f;
    bool mouseLocked = true;

    void movement(Entity* player);
    void setupModelHierarchy();
    void testHierarchyTransformations();
    void handleTestInputs();
    void printHierarchyInfo();
    void AnimateModel(Transform* transform, float timeOffset);
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


    Transform trs2;
    trs2.position = {0, 0, 0.0f};
    trs2.rotationQuat = {0.0f, 0.0f, 0.0f, 0.0f};
    trs2.scale = {5.0f, 5.0f, 5.0f};
    player = new Cube(trs2, new Material(Material::gold()));

    glm::vec3 playerPos = trs2.position;
    playerPos.z -= 2.0f;
    glm::vec3 spotDirection = {0.0f, 0.0f, -1.0f};
    playerLight = new SpotLight(playerPos, spotDirection, {1.0f, 1.0f, 0.8f, 1.0f},
                                5.0f, 55.0f, 1.0f, 0.0045f, 0.00075f);

    Transform trs4;
    trs4.position = {window->getWidth() * .5f, window->getHeight() * .95f, 3};
    trs4.rotationQuat = {0.0f, 0.0f, 0.0f, 0.0f};
    trs4.scale = {static_cast<float>(window->getWidth()), 80, 0};
    floorCollision = new Rectangle(trs4, {0.8f, 0.0f, 1.0f, 0.5f});

    Transform cubeTrs;
    cubeTrs.position = {0.0f, 0.0f, -10.0f};
    cubeTrs.rotationQuat = {10.0f, 10.0f, 10.0f, 10.0f};
    cubeTrs.scale = {10.0f, 10.0f, 10.0f};
    cube = new Cube(cubeTrs, new Material(Material::bronze()));

    trs2.position = {-5, 0, 0.0f};

    pointLight = new PointLight(trs2.position, {1.0f, 1.0f, 1.0f, 1.0f}, 1.0f, 0.0f, 0.0f);
    ambientLight = new AmbientLight({1.0f, 1.0f, 1.0f}, 0.2f);
    collisionManager = new gllib::collisionManager({static_cast<Entity*>(floorCollision)});

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
        model = new Model("models/claire/source/LXG1NDL0BZ814059Q0RW9HZXE.obj", false);
        std::cout << "Scene model loaded successfully with " << model->meshes.size() << " meshes." << std::endl;
        model1 = new Model("models/pochita/pochita.fbx", false);
        std::cout << "Scene model loaded successfully with " << model1->meshes.size() << " meshes." << std::endl;
        model2 = new Model("models/chicken/Chicken1.fbx", false);
        std::cout << "Scene model loaded successfully with " << model2->meshes.size() << " meshes." << std::endl;
        model3 = new Model("models/Backpack/backpack.mtl", false);
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
    model1->transform.scale *= 10;
    model1->transform.position = {10.0f, 0.0f, 10.0f};
    model1->transform.rotationQuat = {0.0f, 0.0f, 0.30f, 0.0f};
    model2->transform.scale *= 10;
    model2->transform.position = {10.0f, 0.0f, 10.0f};
    model2->transform.rotationQuat = {0.0f, 0.0f, 0.30f, 0.0f};
    model3->transform.scale *= 10;
    model3->transform.position = {10.0f, 0.0f, 10.0f};
    model3->transform.rotationQuat = {0.0f, 0.0f, 0.30f, 0.0f};

    srand(time(nullptr));
    window->setTitle("Engine");
}


void Game::update()
{
    // Update
    handleTestInputs();

    // Run hierarchy tests if enabled
    if (hierarchyTestMode)
    {
        testHierarchyTransformations();
    }

    movement(player);
    if (cameraController->getCameraMode() == CameraMode::ThirdPerson)
    {
        camera->updateThirdPersonPosition();
    }

    float rotationSpeed = 30.0f * LibTime::getDeltaTime();
    float angleInRadians = glm::radians(rotationSpeed);

    Quaternion rotationZ;
    rotationZ.w = std::cos(angleInRadians / 2.0f);
    rotationZ.x = 0.0f;
    rotationZ.z = 0.0f;
    rotationZ.y = std::sin(angleInRadians / 2.0f);

    Quaternion cubeRot = cube->getRotationQuat();

    Quaternion newRot;
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
    Renderer::clear();

    Shader::setShaderProgram(shaderProgramLighting);

    ambientLight->apply(shaderProgramLighting);
    pointLight->apply(shaderProgramLighting);
    playerLight->apply(shaderProgramLighting);


    model->draw(getCamera());
    model1->draw(getCamera());
    model2->draw(getCamera());
    model3->draw(getCamera());

    cube->draw();
    player->draw();

    Shader::setShaderProgram(shaderProgramTexture);

    Shader::setShaderProgram(shaderProgramSolidColor);
}

void Game::setupModelHierarchy()
{
    if (!model || !model1 || !model2 || !model3)
        return;

    std::cout << "=== CHECKING LOADED HIERARCHY ===" << std::endl;
    std::cout << "Model has " << model->transform.children.size() << " loaded children" << std::endl;
    std::cout << "Model1 has " << model1->transform.children.size() << " loaded children" << std::endl;
    std::cout << "Model2 has " << model2->transform.children.size() << " loaded children" << std::endl;
    std::cout << "Model3 has " << model3->transform.children.size() << " loaded children" << std::endl;

    // Position the models in the scene
    model->transform.setPosition({0.0f, 0.0f, 0.0f});
    model1->transform.setPosition({30.0f, 0.0f, 0.0f});
    model2->transform.setPosition({-30.0f, 0.0f, 0.0f});
    model3->transform.setPosition({0.0f, 0.0f, 30.0f});

    std::cout << "Hierarchy test will use the loaded node structure from the model files." << std::endl;
}

void Game::testHierarchyTransformations()
{
    if (!hierarchyTestMode || !model)
        return;

    testTimer += gllib::LibTime::getDeltaTime();
    
    // Apply animation to each model with different timing offsets
    AnimateModel(&model->transform, 0.0f);
    if (model1) AnimateModel(&model1->transform, 0.5f);
    if (model2) AnimateModel(&model2->transform, 1.0f);
    if (model3) AnimateModel(&model3->transform, 1.5f);
}

void Game::AnimateModel(Transform* transform, float timeOffset)
{
    if (!transform) return;

    // Function to recursively animate all children
    std::function<void(Transform*, int)> animateChildren = [&](Transform* parent, int depth)
    {
        for (size_t i = 0; i < parent->children.size(); ++i)
        {
            Transform* child = parent->children[i];
            if (!child) continue;

            float adjustedTime = testTimer + timeOffset + (i * 0.2f);

            // Rotate each child around Y axis
            float rotationSpeed = 45.0f + (depth * 15.0f);
            float rotationAngle = rotationSpeed * adjustedTime;

            gllib::Quaternion rotation;
            rotation.w = std::cos(glm::radians(rotationAngle) / 2.0f);
            rotation.x = 0.0f;
            rotation.y = std::sin(glm::radians(rotationAngle) / 2.0f);
            rotation.z = 0.0f;
            rotation.normalize();
            child->setRotation(rotation);

            float scaleVariation = 0.3f * std::sin(adjustedTime * 2.0f);
            float scale = 1.0f + scaleVariation;
            child->setScale({scale, scale, scale});

            if (!child->children.empty())
            {
                animateChildren(transform, 0);
            }
        }
    };

    if (!transform->children.empty())
    {
        animateChildren(transform, 0);
    }

}

void Game::handleTestInputs()
{
    static bool hKeyWasPressed = false;
    static bool mKeyWasPressed = false;
    static bool rKeyWasPressed = false;
    static bool iKeyWasPressed = false;

    // Toggle hierarchy test mode
    if (Input::getKeyPressed(Key_H))
    {
        if (!hKeyWasPressed)
        {
            hierarchyTestMode = !hierarchyTestMode;
            if (hierarchyTestMode)
            {
                setupModelHierarchy();
                std::cout << "Hierarchy test mode ENABLED" << std::endl;
            }
            else
            {
                testTimer = 0.0f;
                // Reset models to original positions when disabling
                if (model)
                {
                    model->transform.setPosition({10.0f, 0.0f, 10.0f});
                    model->transform.setScale({10.0f, 10.0f, 10.0f});
                }
                if (model1)
                {
                    model1->transform.setPosition({10.0f, 0.0f, 10.0f});
                    model1->transform.setScale({10.0f, 10.0f, 10.0f});
                }
                if (model2)
                {
                    model2->transform.setPosition({10.0f, 0.0f, 10.0f});
                    model2->transform.setScale({10.0f, 10.0f, 10.0f});
                }
                if (model3)
                {
                    model3->transform.setPosition({10.0f, 0.0f, 10.0f});
                    model3->transform.setScale({10.0f, 10.0f, 10.0f});
                }
                std::cout << "Hierarchy test mode DISABLED" << std::endl;
            }
            hKeyWasPressed = true;
        }
    }
    else
    {
        hKeyWasPressed = false;
    }

    // Toggle mouse lock
    if (Input::getKeyPressed(Key_M))
    {
        if (!mKeyWasPressed)
        {
            mouseLocked = !mouseLocked;
            std::cout << (mouseLocked ? "Mouse LOCKED" : "Mouse UNLOCKED") << std::endl;
            mKeyWasPressed = true;
        }
    }
    else
    {
        mKeyWasPressed = false;
    }

    // Reset hierarchy test
    if (Input::getKeyPressed(Key_R))
    {
        if (!rKeyWasPressed)
        {
            if (hierarchyTestMode)
            {
                testTimer = 0.0f;
                setupModelHierarchy(); // Reset to initial state
                std::cout << "Hierarchy test RESET" << std::endl;
            }
            rKeyWasPressed = true;
        }
    }
    else
    {
        rKeyWasPressed = false;
    }

    // Print hierarchy info
    if (Input::getKeyPressed(Key_I))
    {
        if (!iKeyWasPressed)
        {
            printHierarchyInfo();
            iKeyWasPressed = true;
        }
    }
    else
    {
        iKeyWasPressed = false;
    }
}

void Game::printHierarchyInfo()
{
    if (!model || !model1 || !model2 || !model3)
    {
        std::cout << "Models not loaded!" << std::endl;
        return;
    }

    std::cout << "\n=== HIERARCHY INFO ===" << std::endl;

    auto printModelInfo = [](const std::string& name, Model* modelPtr)
    {
        std::cout << name << ":" << std::endl;
        std::cout << "  Local Pos: (" << modelPtr->transform.position.x << ", " << modelPtr->transform.position.y <<
            ", " << modelPtr->transform.position.z << ")" << std::endl;
        std::cout << "  Local Scale: (" << modelPtr->transform.scale.x << ", " << modelPtr->transform.scale.y << ", " <<
            modelPtr->transform.scale.z << ")" << std::endl;
        std::cout << "  Children: " << modelPtr->transform.children.size() << std::endl;
        std::cout << "  Parent: " << (modelPtr->transform.parent ? "Yes" : "No") << std::endl;

        glm::mat4 worldMatrix = modelPtr->transform.getTransformMatrix();
        glm::vec3 worldPos = glm::vec3(worldMatrix[3]);
        std::cout << "  World Pos: (" << worldPos.x << ", " << worldPos.y << ", " << worldPos.z << ")" << std::endl;

        std::cout << "  AABB Min: (" << modelPtr->transform.getWorldAABBMin().x << ", " << modelPtr->transform.
            getWorldAABBMin().y << ", " << modelPtr->transform.getWorldAABBMin().z << ")" << std::endl;
        std::cout << "  AABB Max: (" << modelPtr->transform.getWorldAABBMax().x << ", " << modelPtr->transform.
            getWorldAABBMax().y << ", " << modelPtr->transform.getWorldAABBMax().z << ")" << std::endl;
    };

    printModelInfo("Model (root)", model);
    printModelInfo("Model1 (child of Model)", model1);
    printModelInfo("Model2 (child of Model1)", model2);
    printModelInfo("Model3 (child of Model2)", model3);

    std::cout << "=====================\n" << std::endl;
}

void Game::movement(Entity* player)
{
    Transform transform2 = player->getTransform();
    transform2.position.y += 1.f;
    float speed = 80 * LibTime::getDeltaTime();

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

    Transform transform = player->getTransform();
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
