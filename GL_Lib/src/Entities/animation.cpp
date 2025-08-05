#include "animation.h"

#include <iostream>

#include "Core/lib_time.h"


using namespace std;

namespace gllib
{
    Animation::Animation(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale, Color color) :
        Sprite(translation, rotation, scale, color)
    {
        durationInSecs = .0f;
        elapsedTime = .0f;
        paused = false;

        cout << "Created animation.\n";
    }

    Animation::Animation(Transform transform, Color color) :
        Sprite(transform, color)
    {
        durationInSecs = .0f;
        elapsedTime = .0f;
        paused = false;

        cout << "Created animation.\n";
    }

    Animation::~Animation()
    {
        cout << "Destroyed animation.\n";
    }

    void Animation::addFramesFromAtlas(unsigned int textureID, int startX, int startY, int frameWidth, int frameHeight, int columns, int rows)
    {
        for (int y = 0; y < rows; ++y)
        {
            for (int x = 0; x < columns; ++x)
            {
                int offsetX = startX + x * frameWidth;
                int offsetY = startY + y * frameHeight;
                addFrame(textureID, offsetX, offsetY, frameWidth, frameHeight);
            }
        }
    }

    void Animation::addFrames(unsigned int textureID, int frameWidth, int frameHeight, int columns, int rows)
    {
        for (int y = 0; y < rows; ++y)
        {
            for (int x = 0; x < columns; ++x)
            {
                int offsetX = x * frameWidth;
                int offsetY = y * frameHeight;
                addFrame(textureID, offsetX, offsetY, frameWidth, frameHeight);
            }
        }
    }

    void Animation::setDurationInSecs(double durationInSecs)
    {
        this->durationInSecs = durationInSecs;
    }

    void Animation::setAnimationPaused(bool paused) {
        this->paused = paused;
    }

    void Animation::update()
    {
        if (paused) return;
        elapsedTime += LibTime::getDeltaTime();

        double normalizedTime = fmod(elapsedTime, durationInSecs);
        int totalFrames = getFrameCount() + 1;

        if (totalFrames < 1)
            return;

        int framesPerSecond = static_cast<int>(totalFrames / durationInSecs);
        int currentFrame = static_cast<int>(normalizedTime * framesPerSecond) % totalFrames;

        setCurrentFrame(currentFrame);
    }

    void Animation::reset() 
    {
        setCurrentFrame(0);
    }
}
