#ifndef FRAMEDIVIDER_H
#define FRAMEDIVIDER_H

#include "ScreenCapture.h"
#include "averagecolor.h"
#include "framewrapper.h"
#include "math.h"

using namespace std;
using namespace SL;

namespace AC {
    class FrameDivider {

        public:
            FrameDivider();

            vector<QColor> CalculateHorizontalRow(vector<Screen_Capture::ImageBGRA>::iterator imgRef, unsigned int blocksWidth, unsigned int imageWidth, unsigned int offsetHeight, unsigned int depth, unsigned int pixelPerblock);
            vector<QColor> CalculateVerticalRow(vector<Screen_Capture::ImageBGRA>::iterator imgRef, unsigned int blocksHeight, unsigned int imageWidth, unsigned int imageHeight, unsigned int offsetWidth, unsigned int depth, unsigned int pixelPerblock);

            QColor CalculateHorizontalPixel(vector<Screen_Capture::ImageBGRA>::iterator imgRef, unsigned int rowOffset, unsigned int blockPos, unsigned int imageWidth, unsigned int depth, unsigned int pixelPerblockWidth);
            QColor CalculateVerticalPixel(vector<Screen_Capture::ImageBGRA>::iterator imgRef, unsigned int columnOffset, unsigned int blockPos, unsigned int imageWidth, unsigned int imageHeight, unsigned int depth, unsigned int pixelPerblockHeight);

            unsigned int CalculateHorizontalOffset(unsigned int width, unsigned int height, double contentRatio);
            unsigned int CalculateVerticalOffset(unsigned int width, unsigned int height, double contentRatio);
            unsigned int PixelsPerBlockHeight(unsigned int imageHeight, unsigned int blocksHeight);
            unsigned int PixelsPerBlockWith(unsigned int imageWidth, unsigned int blocksWidth);
    };
}

#endif // FRAMEDIVIDER_H
