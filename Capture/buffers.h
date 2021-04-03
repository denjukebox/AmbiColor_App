#ifndef BUFFERS_H
#define BUFFERS_H

#include "ScreenCapture.h"
#include "framewrapper.h"
#include "resultwrapper.h"
#include "Util/statistics.h"
#include <queue>
#include <mutex>

using namespace std;
using namespace SL;

namespace AC
{

    template <class WrapperT>
    class Buffer
    {

    public:
        virtual bool Push(WrapperT* wrapper){
            try {
                lock_guard<mutex> guard(_lock);
                _free.push(wrapper);
                Statistics::Instance().NextQueued(_logType);
                return true;
            }  catch (const exception& ex) {
                return false;
            }
        }

        virtual WrapperT* GetUsed()
        {
            try {
                if(_used.empty())
                    throw exception();
                lock_guard<mutex> guard(_lock);
                auto wrapper = _used.front();
                _used.pop();
                return wrapper;
            }  catch (const exception& ex) {
                return nullptr;
            }
        }

        virtual int GetUsedSize()
        {
            return _used.size();
        }

        virtual bool GetUsedPosition(WrapperT* ref)
        {
            return distance(_used.front(), ref);
        }

        virtual WrapperT* GetFree()
        {
            try {
                if(_free.empty())
                    throw exception();
                lock_guard<mutex> guard(_lock);
                auto wrapper = _free.front();
                _free.pop();
                Statistics::Instance().NextConsumed(_logType);
                return wrapper;
            }  catch (const exception& ex) {
                return nullptr;
            }
        }

        virtual int GetFreeSize()
        {
            return _free.size();
        }

        virtual int GetFreePosition(WrapperT* ref)
        {
            return distance(_free.front(), ref);
        }

        virtual bool Clean(WrapperT* ref)
        {
            try {
                lock_guard<mutex> guard(_lock);
                _used.push(ref);
                return true;
            }   catch (const exception& ex) {
                return false;
            }
        }

    protected:
        const static int BUFFER_STACK_MAX_COUNT = 50;

        mutex _lock;
        queue<WrapperT*> _free;
        queue<WrapperT*> _used;
        array<WrapperT, BUFFER_STACK_MAX_COUNT> _stack;
        Statistics::StatisticType _logType;

        void SyncBufferUse(int usedSize = BUFFER_STACK_MAX_COUNT){
            lock_guard<mutex> guard(_lock);
            for(auto pos = 0; pos < usedSize; pos++){
                _used.push(&_stack[pos]);
            }
        }

        void FillBuffers(int stackSize = BUFFER_STACK_MAX_COUNT){
            lock_guard<mutex> guard(_lock);
            for(auto pos = 0; pos < stackSize; pos++){
                _stack[pos] = WrapperT();
            }
        }
    };

    class FrameManager : public Buffer<FrameWrapper>
    {

    public:
        FrameManager()
        {
            FillBuffers();
            SyncBufferUse();
        }

        bool Queue(const Screen_Capture::Image &img, const Screen_Capture::Monitor &monitor);

    protected:
        Statistics::StatisticType _logType = Statistics::StatisticType::Frame;
    };

    class ResultManager : public Buffer<ResultWrapper>
    {

    public:
        ResultManager()
        {
            FillBuffers();
            SyncBufferUse();
        }

        bool Queue(vector<QColor> topColors,
                        vector<QColor> bottomColors,
                        vector<QColor> leftColors,
                        vector<QColor> rightColors);        
    protected:
        Statistics::StatisticType _logType = Statistics::StatisticType::Result;
    };

    class TimeManager : public Buffer<ResultWrapper>
    {

    public:
        TimeManager(unsigned long depth){
            _depth = depth;
            FillBuffers(depth);
            SyncBufferUse(depth);
        }

        void Rotate(unsigned long distance);

        bool Queue(vector<QColor> topColors,
                        vector<QColor> bottomColors,
                        vector<QColor> leftColors,
                        vector<QColor> rightColors);
    protected:
        Statistics::StatisticType _logType = Statistics::StatisticType::Time;
        unsigned long _depth;
    };
}

#endif // BUFFERS_H
