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
        Buffer()
        {
            FillBuffers();
            SyncBufferUse();
        }

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

        virtual int GetPosition(WrapperT* ref)
        {
            return distance(&_stack.front(), ref);
        }

    protected:
        const static int BUFFER_STACK_MAX_COUNT = 10;

        mutex _lock;
        queue<WrapperT*> _free;
        queue<WrapperT*> _used;
        array<WrapperT, BUFFER_STACK_MAX_COUNT> _stack;
        Statistics::StatisticType _logType;

        void SyncBufferUse(){
            lock_guard<mutex> guard(_lock);
            for(auto pos = 0; pos < BUFFER_STACK_MAX_COUNT; pos++){
                _used.push(&_stack[pos]);
            }
        }

        void FillBuffers(){
            lock_guard<mutex> guard(_lock);
            for(auto pos = 0; pos < BUFFER_STACK_MAX_COUNT; pos++){
                _stack[pos] = WrapperT();
            }
        }
    };

    class FrameManager : public Buffer<FrameWrapper>
    {

    public:
        FrameManager();
        bool Queue(const Screen_Capture::Image &img);
    };

    class ResultManager : public Buffer<ResultWrapper>
    {

    public:
        ResultManager();
        bool Queue(vector<QColor> topColors,
                        vector<QColor> bottomColors,
                        vector<QColor> leftColors,
                        vector<QColor> rightColors);
    };
}

#endif // BUFFERS_H
