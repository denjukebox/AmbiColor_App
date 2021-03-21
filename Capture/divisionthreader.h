#ifndef DIVISIONTHREADER_H
#define DIVISIONTHREADER_H

#include <iostream>
#include <thread>
#include <chrono>
#include <future>
#include <typeinfo>
#include "buffermanager.h"
#include "framedivider.h"
#include "teensythreader.h"
#include "Util/statistics.h"
#include "Util/settings.h"
#include "Util/logger.h"
#include "enums.h"

using namespace std;
using namespace SL;

namespace AC {
    class DivisionThreader
    {

    public:
        DivisionThreader();

        void Stop();
        void Start();
        bool IsRunning();

    private:
        static void Thread(FrameDivider *divider, BufferManager *manager, Settings *settings, bool *threadActive);
        static void DivideFrame(FrameDivider *divider, FrameWrapper *frame, BufferManager *manager, Settings *settings);

        static vector<QColor> CalculateTop(FrameDivider *divider, FrameWrapper *frame, unsigned int blocksWidth, unsigned int depth, double contentRatio);
        static vector<QColor> CalculateBottom(FrameDivider *divider, FrameWrapper *frame, unsigned int blocksWidth, unsigned int depth, double contentRatio);
        static vector<QColor> CalculateLeft(FrameDivider *divider, FrameWrapper *frame, unsigned int blocksHeight, unsigned int depth, double contentRatio);
        static vector<QColor> CalculateRight(FrameDivider *divider, FrameWrapper *frame, unsigned int blocksHeight, unsigned int depth, double contentRatio);

        bool _threadActive = false;
        std::thread _processThread;

        BufferManager *_manager = &BufferManager::Instance();
        Settings *_settings = &Settings::Instance();
        FrameDivider _divider = FrameDivider();
    };
}

#endif // DIVISIONTHREADER_H
