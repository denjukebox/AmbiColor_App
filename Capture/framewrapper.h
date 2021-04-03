#ifndef FRAMEWRAPPER_H
#define FRAMEWRAPPER_H

#include "ScreenCapture.h"
#include "vector"

using namespace std;
using namespace SL;

namespace AC {
    class FrameWrapper
    {

    public:
        FrameWrapper();

        vector<Screen_Capture::ImageBGRA>::iterator GetFrameBegin();

        void CopyImageToBuffer(const Screen_Capture::Image &img, const Screen_Capture::Monitor &monitor);

        Screen_Capture::Monitor GetMonitor();
        unsigned int GetFrameWidth();
        unsigned int GetFrameHeight();

    private:
        vector<Screen_Capture::ImageBGRA> _frameBuffer = vector<Screen_Capture::ImageBGRA>();
        Screen_Capture::Monitor _monitor;
        unsigned int _frameWidth = 0;
        unsigned int _frameHeight = 0;
    };
}

#endif // FRAMEWRAPPER_H
