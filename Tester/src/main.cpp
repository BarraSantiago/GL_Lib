#include "Core/base_game.h"

#include <iostream>

#include "Math/transform.h"
#include "Rendering/renderer.h"


using namespace std;
using namespace gllib;
// Para el final de diciembre, incluir un modelo de entre planos que muestre que cuando cruza el plano, se deja de dibujar
// Tambien incluir controles para ambas partes del modelo
class Game : public BaseGame
{
private:
    float playerSpeed = 250.0f;
    float tankSpeed = 75;
    std::vector<BSPPlane> separatingPlanes;

    BSPSystem bspSystem;
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
    void handleTestInputs();
    void buildBSP();
    void autoLoadModels();

protected:
    void init() override;
    void drawObjects();
    void update() override;
    void uninit() override;

public:
    Game();
    ~Game() override;
};
bool showAABB = true;

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
    camera->setDistance(150.0f);
    camera->setHeight(2.0f);
    camera->setPerspective(45.0f, window->getWidth() / (float)window->getHeight(), 0.1f, 1000.0f);

    camera->setRotation(0.0f, 0.0f);

    //try
    //{
        model1 = new Model("models/planes/wall2.fbx", false);
        std::cout << "Scene model loaded successfully with " << model1->meshes.size() << " meshes." << '\n';
        model2 = new Model("models/tank_1.fbx", false);
        std::cout << "Scene model loaded successfully with " << model2->meshes.size() << " meshes." << '\n';
    //    model3 = new Model("models/planes/wall1.fbx", false);
    //    
    //    
    //    //model = new Model("models/claire/source/LXG1NDL0BZ814059Q0RW9HZXE.obj", false);
    //    //std::cout << "Scene model loaded successfully with " << model->meshes.size() << " meshes." << '\n';
    //    //model3 = new Model("models/Backpack/backpack.mtl", false);
    //    //std::cout << "Scene model loaded successfully with " << model3->meshes.size() << " meshes." << '\n';
    //}
    //catch (const std::exception& e)
    //{
    //    std::cout << "Failed to load scene model: " << e.what() << '\n';
    //    model = nullptr;
    //}
//
    //glm::vec3 rotationEuler = {0.0f, 0.0f, 90.0f};
    //model1->transform.scale *= .1;
    //model1->transform.position = {0.0f, 0.0f, 0.0f};
    //model1->transform.setRotation(rotationEuler);
    //
    //rotationEuler = {270.0f, 0.0f, 0.0f};
    //
    //model2->transform.scale *= .5;
    //model2->transform.position = {-20.0f, 0.0f, 0.0f};
    //model2->transform.setRotation(rotationEuler);
    model2->setMaterial(new Material(Material::emerald()));

    if (model2->transform.children.size() > 0)
    {
        model2->setMaterialForTransform(model2->transform.children[0], new Material(Material::gold()));
    }
    if (model2->transform.children.size() > 1)
    {
        model2->setMaterialForTransform(model2->transform.children[1], new Material(Material::ruby()));
    }
    if (model2->transform.children.size() > 2)
    {
        model2->setMaterialForTransform(model2->transform.children[2], new Material(Material::bronze()));
        model2->setMaterialForTransform(model2->transform.children[2]->children[0], new Material(Material::ruby()));
        model2->setMaterialForTransform(model2->transform.children[2]->children[1], new Material(Material::ruby()));
    }
    
    //autoLoadModels();
    buildBSP();
    
    srand(time(nullptr));
    window->setTitle("Engine - BSP Test (IJKL to move chicken)");
    
    std::cout << "=== BSP SETUP ===" << '\n';
    std::cout << "Wall (partition) at X=0" << '\n';
    //std::cout << "Tank starts at X=" << model2->transform.position.x << '\n';
    std::cout << "Use IJKL keys to move chicken across the wall" << '\n';
    //rotationEuler = {90.0f, 0.0f, 90.0f};
    //model1->transform.setRotation(rotationEuler);
}


void Game::update()
{
    // Update
    handleTestInputs();

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

    drawObjects();
}


void Game::drawObjects()
{
    Renderer::clear();
    Shader::setShaderProgram(shaderProgramLighting);

    ambientLight->apply(shaderProgramLighting);
    pointLight->apply(shaderProgramLighting);
    playerLight->apply(shaderProgramLighting);

    bspSystem.render(*camera);
    
    Shader::setShaderProgram(shaderProgramSolidColor);
    bspSystem.renderDebug(*camera, showAABB);

    Shader::setShaderProgram(shaderProgramLighting);
    cube->draw();
    player->draw();

    Shader::setShaderProgram(shaderProgramTexture);
    Shader::setShaderProgram(shaderProgramSolidColor);
}



void Game::handleTestInputs()
{
    static bool bKeyWasPressed = false;
    
    // Toggle AABB visualization
    if (Input::getKeyPressed(Key_B))
    {
        if (!bKeyWasPressed)
        {
            showAABB = !showAABB;
            std::cout << (showAABB ? "AABB visualization ENABLED" : "AABB visualization DISABLED") << '\n';
            bKeyWasPressed = true;
        }
    }
    else
    {
        bKeyWasPressed = false;
    }

    static float lastReportedX = model2->transform.position.x;
    
    if (Input::getKeyPressed(Key_I))
    {
        glm::vec3 forward = {tankSpeed * LibTime::getDeltaTime(), 0, 0.0f};
        model2->transform.setPosition(model2->transform.position + forward);
    }
    if (Input::getKeyPressed(Key_K))
    {
        glm::vec3 forward = {tankSpeed * LibTime::getDeltaTime(), 0, 0.0f};
        model2->transform.setPosition(model2->transform.position - forward);
    }
    if (Input::getKeyPressed(Key_J))
    {
        glm::vec3 right = {0.0f, 0, tankSpeed * LibTime::getDeltaTime()};
        model2->transform.setPosition(model2->transform.position + right);
    }
    if (Input::getKeyPressed(Key_L))
    {
        glm::vec3 right = {0.0f, 0, tankSpeed * LibTime::getDeltaTime()};
        model2->transform.setPosition(model2->transform.position - right);
    }
    const float rotationSpeed = 90.0f;
    if (Input::getKeyPressed(Key_U))
    {
        glm::vec3 left = model2->transform.children[2]->getRotationEuler();
        left.z += -rotationSpeed * LibTime::getDeltaTime();
        model2->transform.children[2]->setRotation(left);
    }
    
    if (Input::getKeyPressed(Key_O))
    {
        glm::vec3 right = model2->transform.children[2]->getRotationEuler();
        right.z += rotationSpeed * LibTime::getDeltaTime();
        model2->transform.children[2]->setRotation(right);
    }

    if (Input::getKeyPressed(Key_P))
    {
        glm::vec3 up = model2->transform.children[2]->getRotationEuler();
        up.x += rotationSpeed * LibTime::getDeltaTime();
        model2->transform.children[2]->setRotation(up);
    }
    if (Input::getKeyPressed(Key_Y))
    {
        glm::vec3 up = model2->transform.children[2]->getRotationEuler();
        up.x -= rotationSpeed * LibTime::getDeltaTime();
        model2->transform.children[2]->setRotation(up);
    }
    float currentX = model2->transform.position.x;
    if ((lastReportedX < 0.0f && currentX >= 0.0f) || (lastReportedX >= 0.0f && currentX < 0.0f))
    {
        std::cout << "Chicken crossed wall! Now at X=" << currentX 
                  << " (Camera at X=" << camera->getPosition().x << ")" << '\n';
        lastReportedX = currentX;
    }
    
}

void Game::buildBSP()
{
    for (Model* mod : Model::getLoadedModels())
    {
        bspSystem.addModel(mod);
        
        std::vector<BSPPlane> planes = mod->createBSPPlanesFromNodes();
        for (const BSPPlane& plane : planes)
        {
            bspSystem.addPlane(plane);
        }
    }
    
    bspSystem.buildBSP();
}

void Game::movement(Entity* player)
{
    Transform transform2 = player->getTransform();
    transform2.position.y += 1.f;
    playerSpeed = 80 * LibTime::getDeltaTime();

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
        moveVector = cameraFront * playerSpeed;
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
        moveVector = -cameraFront * playerSpeed;
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
        moveVector = -cameraRight * playerSpeed;
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
        moveVector = cameraRight * playerSpeed;
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


#include <filesystem>

void Game::autoLoadModels()
{
    namespace fs = std::filesystem;
    std::string modelsPath = "models";
    
    if (!fs::exists(modelsPath) || !fs::is_directory(modelsPath))
    {
        std::cout << "Models folder not found!" << '\n';
        return;
    }
    
    for (const auto& entry : fs::recursive_directory_iterator(modelsPath))
    {
        if (!entry.is_regular_file())
            continue;
        
        std::string ext = entry.path().extension().string();
        for (char& c : ext)
            c = static_cast<char>(std::tolower(c));
        
        if (ext == ".obj" || ext == ".fbx" || ext == ".gltf" || ext == ".glb")
        {
            try
            {
                std::string path = entry.path().string();
                std::cout << "Auto-loading: " << path << '\n';
                new Model(path, false);
            }
            catch (const std::exception& e)
            {
                std::cerr << "Failed to load " << entry.path() << ": " << e.what() << '\n';
            }
        }
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
