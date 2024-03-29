#ifndef DIVISIONTHREADER_H
#define DIVISIONTHREADER_H

#include <iostream>
#include <thread>
#include <chrono>
#include <future>
#include <typeinfo>
#include "buffers.h"
#include "framedivider.h"
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
        DivisionThreader(FrameManager *frameManager, ResultManager *resultManager, TimeManager *timeManager);

        void Stop();
        void Start();
        bool IsRunning();

    private:
        static void Thread(FrameDivider *divider, FrameManager *frameManager, ResultManager *resultManager, TimeManager *timeManager, Settings *settings, bool *threadActive);
        static void DivideFrame(FrameDivider *divider, FrameWrapper *frame, ResultManager *resultManager, TimeManager *timeManager, Settings *settings);

        static void TimeSmoothing(vector<QColor> *top, vector<QColor> *bottom, vector<QColor> *left, vector<QColor> *right, TimeManager *timeManager, unsigned long smoothing);
        static void CalculateTimeSmoothing(vector<WeightedAverageColor> *average, vector<QColor>::iterator result, unsigned long size, unsigned long smoothing);
        static void ApplyTimeSmoothing(vector<WeightedAverageColor> *average, vector<QColor>::iterator result, unsigned long size);

        static vector<QColor> CalculateTop(FrameDivider *divider, FrameWrapper *frame, unsigned int blocksWidth, unsigned int depth, double contentRatio);
        static vector<QColor> CalculateBottom(FrameDivider *divider, FrameWrapper *frame, unsigned int blocksWidth, unsigned int depth, double contentRatio);
        static vector<QColor> CalculateLeft(FrameDivider *divider, FrameWrapper *frame, unsigned int blocksHeight, unsigned int depth, double contentRatio);
        static vector<QColor> CalculateRight(FrameDivider *divider, FrameWrapper *frame, unsigned int blocksHeight, unsigned int depth, double contentRatio);

        bool _threadActive = false;
        std::thread _processThread;

        FrameManager *_frameManager;
        ResultManager *_resultManager;        
        TimeManager *_timeManager;

        Settings *_settings = &Settings::Instance();
        FrameDivider _divider = FrameDivider();
    };
}

#endif // DIVISIONTHREADER_H
