﻿#include "animation.h"

#include <iostream>

#include "lib_time.h"
#include "loader.h"

using namespace std;

namespace gllib
{
    Animation::Animation(Vector3 translation, Vector3 rotation, Vector3 scale, Color color) :
        Sprite(translation, rotation, scale, color)
    {
        durationInSecs = .0f;
        elapsedTime = .0f;

        cout << "Created animation.\n";
    }

    Animation::Animation(Transform transform, Color color) :
        Sprite(transform, color)
    {
        durationInSecs = .0f;
        elapsedTime = .0f;

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

    void Animation::update()
    {
        elapsedTime += LibTime::getDeltaTime();

        double normalizedTime = fmod(elapsedTime, durationInSecs);
        int totalFrames = getFrameCount() + 1;

        if (totalFrames < 1)
            return;

        int framesPerSecond = static_cast<int>(totalFrames / durationInSecs);
        int currentFrame = static_cast<int>(normalizedTime * framesPerSecond) % totalFrames;

        setCurrentFrame(currentFrame);
    }
}
