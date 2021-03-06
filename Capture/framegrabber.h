#ifndef FRAMEGRABBER_H
#define FRAMEGRABBER_H

#include "ScreenCapture.h"
#include "framedivider.h"
#include "buffers.h"
#include "Util/statistics.h"
#include "Util/settings.h"
#include "Util/logger.h"
#include "Views/configuration.h"
#include "ScreenCapture.h"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <climits>
#include <iostream>
#include <locale>
#include <string>
#include <thread>
#include <vector>

using namespace std;
using namespace SL;

namespace AC {
    class FrameGrabber
    {

    public:
        FrameGrabber(FrameManager *frameManager);
        ~FrameGrabber();

        void StartCapture();
        void StopCapture();

        bool IsRunning();
        void SetMonitor(Screen_Capture::Monitor monitor);

    private:
        void LogMonitor(Screen_Capture::Monitor monitor);
        FrameManager *_frameManager;
        Settings *_settings = &Settings::Instance();

        shared_ptr<Screen_Capture::IScreenCaptureManager> _grabber;
        static void ProcessCapturedFrame(FrameManager *frameManager, const Screen_Capture::Image &img, const Screen_Capture::Monitor &monitor);

        Screen_Capture::Monitor _monitor = Screen_Capture::Monitor();

        bool _isRunning = false;
    };
}

#endif // FRAMEGRABBER_H
