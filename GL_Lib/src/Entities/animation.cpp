#include "animation.h"

#include <cmath>
#include <iostream>

#include "Core/lib_time.h"

using namespace std;

namespace gllib
{
    Animation::Animation(Vector3 translation, Vector3 rotation, Vector3 scale, Color color) :
        sprite(new Sprite(translation, rotation, scale, color)),
        ownsSprite(true),
        durationInSecs(0.0),
        elapsedTime(0.0),
        paused(false)
    {
        cout << "Created animation.\n";
    }

    Animation::Animation(Transform transform, Color color) :
        sprite(new Sprite(transform, color)),
        ownsSprite(true),
        durationInSecs(0.0),
        elapsedTime(0.0),
        paused(false)
    {
        cout << "Created animation.\n";
    }

    Animation::Animation(Sprite* sprite, bool takeOwnership) :
        sprite(sprite),
        ownsSprite(takeOwnership),
        durationInSecs(0.0),
        elapsedTime(0.0),
        paused(false)
    {
        cout << "Created animation.\n";
    }

    Animation::~Animation()
    {
        if (ownsSprite) {
            delete sprite;
        }
        sprite = nullptr;
        cout << "Destroyed animation.\n";
    }

    void Animation::addFramesFromAtlas(unsigned int textureID, int startX, int startY, int frameWidth, int frameHeight, int columns, int rows)
    {
        if (sprite == nullptr) return;

        for (int y = 0; y < rows; ++y)
        {
            for (int x = 0; x < columns; ++x)
            {
                int offsetX = startX + x * frameWidth;
                int offsetY = startY + y * frameHeight;
                sprite->addFrame(textureID, offsetX, offsetY, frameWidth, frameHeight);
            }
        }
    }

    void Animation::addFrames(unsigned int textureID, int frameWidth, int frameHeight, int columns, int rows, int columnsOffset, int rowsOffset)
    {
        if (sprite == nullptr) return;

        for (int y = rowsOffset; y < rows; ++y)
        {
            for (int x = columnsOffset; x < columns; ++x)
            {
                int offsetX = x * frameWidth;
                int offsetY = y * frameHeight;
                sprite->addFrame(textureID, offsetX, offsetY, frameWidth, frameHeight);
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
        if (paused || sprite == nullptr || durationInSecs <= 0.0) return;

        elapsedTime += LibTime::getDeltaTime();
        int totalFrames = sprite->getCurrentAnimationFrameCount();
        if (totalFrames < 1) return;

        double normalizedTime = fmod(elapsedTime, durationInSecs);
        double normalizedProgress = normalizedTime / durationInSecs;
        int currentFrame = static_cast<int>(normalizedProgress * static_cast<double>(totalFrames)) % totalFrames;
        sprite->setCurrentFrame(currentFrame);
    }

    void Animation::reset() 
    {
        elapsedTime = 0.0;
        if (sprite == nullptr) return;
        sprite->setCurrentFrame(0);
    }

    Sprite* Animation::getSprite() const
    {
        return sprite;
    }

    void Animation::draw()
    {
        if (sprite == nullptr) return;
        sprite->draw();
    }

    Transform Animation::getTransform() const
    {
        if (sprite == nullptr) return {};
        return sprite->getTransform();
    }

    void Animation::move(Vector3 direction)
    {
        if (sprite == nullptr) return;
        sprite->move(direction);
    }

    void Animation::setRotationEuler(const Vector3& rotation)
    {
        if (sprite == nullptr) return;
        sprite->setRotationEuler(rotation);
    }

    void Animation::setMirroredX(bool mirrored)
    {
        if (sprite == nullptr) return;
        sprite->setMirroredX(mirrored);
    }

    void Animation::setMirroredY(bool mirrored)
    {
        if (sprite == nullptr) return;
        sprite->setMirroredY(mirrored);
    }

    void Animation::setCurrentAnimation(int animation)
    {
        if (sprite == nullptr) return;
        sprite->setCurrentAnimation(animation);
    }

    void Animation::setCurrentFrame(unsigned int frame)
    {
        if (sprite == nullptr) return;
        sprite->setCurrentFrame(frame);
    }
}
