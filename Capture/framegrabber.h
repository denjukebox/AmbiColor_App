#ifndef FRAMEGRABBER_H
#define FRAMEGRABBER_H

#include "ScreenCapture.h"
#include "framedivider.h"
#include "buffermanager.h"
#include "Util/statistics.h"
#include "Util/settings.h"
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
        FrameGrabber();
        ~FrameGrabber();

        void StartCapture();
        void StopCapture();

        bool IsRunning();
        void SetMonitor(Screen_Capture::Monitor monitor);

    private:
        BufferManager *_manager = &BufferManager::Instance();
        Settings *_settings = &Settings::Instance();

        shared_ptr<Screen_Capture::IScreenCaptureManager> _grabber;
        static void ProcessCapturedFrame(BufferManager *manager, const Screen_Capture::Image &img, const Screen_Capture::Monitor &monitor);

        Screen_Capture::Monitor _monitor = Screen_Capture::Monitor();

        bool _isRunning = false;
    };
}

#endif // FRAMEGRABBER_H
