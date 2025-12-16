#pragma once
#include <map>
#include <string>

#include "../Math/transform.h"
#include "sprite.h"

namespace gllib
{
    // TODO REMOVE SPRITE INHERITANCE
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
        void addFrames(unsigned int textureID, int frameWidth, int frameHeight, int columns, int rows, int columnsOffset, int rowsOffset);
        void setDurationInSecs(double durationInSecs);
        void setAnimationPaused(bool paused);

        void update();
        /// <summary>
        /// Sets the current frame to the first frame.
        /// </summary>
        void reset();
    };
}
