#include "divisionthreader.h"

AC::DivisionThreader::DivisionThreader(FrameManager *frameManager, ResultManager *resultManager)
{
    _frameManager = frameManager;
    _resultManager = resultManager;
}

void AC::DivisionThreader::Start(){
    if(_threadActive)
        return;

    _threadActive = true;
    _processThread = thread(Thread, &_divider, _frameManager, _resultManager, _timeManager, _settings, &_threadActive);
    Logger::WriteActivity(typeid(this), "Started" );
}

void AC::DivisionThreader::Stop(){
    if(!_threadActive)
        return;

    _threadActive = false;
    _processThread.join();
    Logger::WriteActivity(typeid(this), "Stopped");
}

bool AC::DivisionThreader::IsRunning(){
    return _threadActive;
}

void AC::DivisionThreader::Thread(FrameDivider *divider, FrameManager *frameManager, ResultManager *resultManager, ResultManager *timeManager, Settings *settings, bool *threadActive)
{
     while(*threadActive){
         auto frame = frameManager->GetFree();
         auto start = chrono::high_resolution_clock::now();
         if(frame != nullptr)
            DivideFrame(divider, frame, resultManager, timeManager, settings);
         frameManager->Clean(frame);
         auto duration = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start);
         if(duration < settings->GetCaptureRate())
             this_thread::sleep_for(settings->GetCaptureRate() - duration);
     }
}

void AC::DivisionThreader::DivideFrame(FrameDivider *divider, FrameWrapper *frame, ResultManager *resultManager, ResultManager *timeManager, Settings *settings)
{
        auto width = settings->GetWidth();
        auto height = settings->GetHeight();
        auto depth = settings->GetDepth();
        auto ratio =  settings->GetContentRatio();
        auto timeSmoothing = settings->GetTimeSmoothing();

        vector<QColor> top;
        vector<QColor> bottom;
        vector<QColor> left;
        vector<QColor> right;
        //auto time = chrono::high_resolution_clock::now();
        if(settings->GetIsDividedAsync())
        {
            auto asyncTop = async(launch::async, CalculateTop, divider, frame, width, depth, ratio);
            auto asyncBottom = async(launch::async, CalculateBottom, divider, frame, width, depth, ratio);
            auto asyncLeft = async(launch::async, CalculateLeft, divider, frame, height, depth, ratio);
            auto asyncRight = async(launch::async, CalculateRight, divider, frame, height, depth, ratio);

            top = asyncTop.get();
            bottom = asyncBottom.get();
            left = asyncLeft.get();
            right = asyncRight.get();
        }
        else
        {
            top = CalculateTop( divider, frame, width, depth, ratio);
            bottom = CalculateBottom( divider, frame, width, depth, ratio);
            left = CalculateLeft( divider, frame, height, depth, ratio);
            right = CalculateRight( divider, frame, height, depth, ratio);
        }

        //auto spend = chrono::duration_cast<chrono::milliseconds>(time - chrono::high_resolution_clock::now());

        if(timeSmoothing != 0){
            // Add raw frame to time stack if not recursive
            if(!settings->GetIsRecursiveSmoothing())
                timeManager->Queue(top,bottom,left,right);

            ResultWrapper* pastframe;
            vector<WheightedAverageColor> topAverage;
            vector<WheightedAverageColor> bottomAverage;
            vector<WheightedAverageColor> leftAverage;
            vector<WheightedAverageColor> rightAverage;

            for(auto pos = 0; pos < timeSmoothing; pos++ ){
                pastframe = timeManager->GetFree();
                if(pastframe == nullptr){
                    timeManager->Queue(top,bottom,left,right);
                }
                else{
                    CalculateTimeSmoothing(&topAverage, timeSmoothing, pastframe->GetTopBegin(), pastframe->GetTopSize());
                    CalculateTimeSmoothing(&bottomAverage, timeSmoothing, pastframe->GetBottomBegin(), pastframe->GetBottomSize());
                    CalculateTimeSmoothing(&leftAverage, timeSmoothing, pastframe->GetLeftBegin(), pastframe->GetLeftSize());
                    CalculateTimeSmoothing(&rightAverage, timeSmoothing, pastframe->GetRightBegin(), pastframe->GetRightSize());
                    timeManager->Clean(pastframe);
                }
            }

            ApplyTimeSmoothing(&topAverage, top.begin(), top.size());
            ApplyTimeSmoothing(&bottomAverage, bottom.begin(), bottom.size());
            ApplyTimeSmoothing(&leftAverage, left.begin(), left.size());
            ApplyTimeSmoothing(&rightAverage, right.begin(), right.size());

            // Add smooth frame to time stack if recursive
            if(settings->GetIsRecursiveSmoothing())
                timeManager->Queue(top,bottom,left,right);
        }

        resultManager->Queue(top,bottom,left,right);
}

void  AC::DivisionThreader::CalculateTimeSmoothing(vector<WheightedAverageColor> *average, unsigned long int depth, vector<QColor>::iterator result, unsigned long size)
{
    for(unsigned long pos = 0; pos < size; pos++){
        if(average->size() != size){
            average->push_back(WheightedAverageColor(depth));
        }
        average->at(pos).AddToAverage(&result[pos]);
    }
}

void AC::DivisionThreader::ApplyTimeSmoothing(vector<WheightedAverageColor> *average, vector<QColor>::iterator result, unsigned long size){
    for(unsigned long pos = 0; pos < size; pos++){
        result[pos] = average->at(pos).GetAverage();
    }
}

vector<QColor> AC::DivisionThreader::CalculateTop(FrameDivider *divider, FrameWrapper *frame, unsigned int blocksWidth, unsigned int depth, double contentRatio)
{
    auto verticalOffset = divider->CalculateVerticalOffset(frame->GetFrameWidth(), frame->GetFrameHeight(), contentRatio);
    return divider->CalculateHorizontalRow(
                                frame->GetFrameBegin(),
                                blocksWidth,
                                frame->GetFrameWidth(),
                                verticalOffset,
                                depth,
                                divider->PixelsPerBlockWith(frame->GetFrameWidth(), blocksWidth));
}

vector<QColor> AC::DivisionThreader::CalculateBottom(FrameDivider *divider, FrameWrapper *frame, unsigned int blocksWidth, unsigned int depth, double contentRatio)
{
    auto verticalOffset = frame->GetFrameHeight() - depth - divider->CalculateVerticalOffset(frame->GetFrameWidth(), frame->GetFrameHeight(), contentRatio);
    return divider->CalculateHorizontalRow(
                                frame->GetFrameBegin(),
                                blocksWidth,
                                frame->GetFrameWidth(),
                                verticalOffset,
                                depth,
                                divider->PixelsPerBlockWith(frame->GetFrameWidth(), blocksWidth));
}

vector<QColor> AC::DivisionThreader::CalculateLeft(FrameDivider *divider, FrameWrapper *frame, unsigned int blocksHeight, unsigned int depth, double contentRatio)
{
    auto horizontalOffset = divider->CalculateHorizontalOffset(frame->GetFrameWidth(), frame->GetFrameHeight(), contentRatio);
    return divider->CalculateVerticalRow(
                                frame->GetFrameBegin(),
                                blocksHeight,
                                frame->GetFrameWidth(),
                                frame->GetFrameHeight(),
                                horizontalOffset,
                                depth,
                                divider->PixelsPerBlockHeight(frame->GetFrameHeight(), blocksHeight));
}

vector<QColor> AC::DivisionThreader::CalculateRight(FrameDivider *divider, FrameWrapper *frame, unsigned int blocksHeight, unsigned int depth, double contentRatio)
{
    auto horizontalOffset = frame->GetFrameWidth() - depth - divider->CalculateHorizontalOffset(frame->GetFrameWidth(), frame->GetFrameHeight(), contentRatio);
    return divider->CalculateVerticalRow(
                                frame->GetFrameBegin(),
                                blocksHeight,
                                frame->GetFrameWidth(),
                                frame->GetFrameHeight(),
                                horizontalOffset,
                                depth,
                                divider->PixelsPerBlockHeight(frame->GetFrameHeight(), blocksHeight));
}
