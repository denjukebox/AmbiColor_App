#ifndef DIVIDETHREADER_H
#define DIVIDETHREADER_H

#include "buffermanager.h"
#include "framedivider.h"
#include "Util/statistics.h"
#include "teensythreader.h"
#include "Util/settings.h"
#include "enums.h"
#include <thread>
#include <chrono>

using namespace std;
using namespace SL;

namespace AC {
    class PreviewThreader
    {

    public:
        PreviewThreader();

        void StopThread();
        void StartThread();

    private:
        static void DivideFrame(FrameDivider *divider, BufferManager *manager, Settings *settings, bool *threadActive);

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

#endif // DIVIDETHREADER_H
