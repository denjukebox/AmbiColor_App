#ifndef BUFFERMANAGER_H
#define BUFFERMANAGER_H

#include "ScreenCapture.h"
#include "framewrapper.h"
#include "resultwrapper.h"
#include <queue>
#include <mutex>

using namespace std;
using namespace SL;

namespace AC
{
    class BufferManager
    {

    public:
        BufferManager();

        static BufferManager &Instance() {static BufferManager buffer; return buffer;}

        bool QueueFrame(const Screen_Capture::Image &img);
        FrameWrapper* GetFreeFrame();
        void CleanFrame(FrameWrapper* frameRef);
        void ReuseFrame(FrameWrapper* frameRef);
        int GetPosition(FrameWrapper* frameRef);

        void QueueResult(vector<QColor> topColors,
                        vector<QColor> bottomColors,
                        vector<QColor> leftColors,
                        vector<QColor> rightColors);
        ResultWrapper* GetFreeResult();
        void CleanResult(ResultWrapper* resultRef);
        void ReuseResult(ResultWrapper* resultRef);
        int GetPosition(ResultWrapper* resultRef);

    private:
        const static int BUFFER_STACK_MAX_COUNT = 10;

        void SyncBufferUse();
        void FillBuffers();

        mutex _frameLock;
        queue<FrameWrapper *> _freeFrames;
        queue<FrameWrapper *> _usedFrames;
        array<FrameWrapper, BUFFER_STACK_MAX_COUNT> _framesStack;

        mutex _resultLock;
        queue<ResultWrapper *> _freeResults;
        queue<ResultWrapper *> _usedResults;
        array<ResultWrapper, BUFFER_STACK_MAX_COUNT> _resultsStack;
    };
}

#endif // BUFFERMANAGER_H
