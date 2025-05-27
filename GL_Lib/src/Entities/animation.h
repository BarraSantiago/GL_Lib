#pragma once
#include <map>
#include <string>

#include "sprite.h"

namespace gllib
{
    class DLLExport Animation : public Sprite {
    private:
        double durationInSecs;
        double elapsedTime;
        bool paused;

    public:
        Animation(Vector3 translation, Vector3 rotation, Vector3 scale, Color color);
        Animation(Transform transform, Color color);
        virtual ~Animation() override;

        void addFramesFromAtlas(unsigned int textureID, int startX, int startY, int frameWidth, int frameHeight, int columns, int rows);
        void addFrames(unsigned int textureID, int frameWidth, int frameHeight, int columns, int rows);
        void setDurationInSecs(double durationInSecs);
        void setAnimationPaused(bool paused);

        void update();
        /// <summary>
        /// Sets the current frame to the first frame.
        /// </summary>
        void reset();
    };
}
