#include "divisionthreader.h"

AC::DivisionThreader::DivisionThreader(FrameManager *frameManager, ResultManager *resultManager, TimeManager *timeManager)
{
    _frameManager = frameManager;
    _resultManager = resultManager;
    _timeManager = timeManager;
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

void AC::DivisionThreader::Thread(FrameDivider *divider, FrameManager *frameManager, ResultManager *resultManager, TimeManager *timeManager, Settings *settings, bool *threadActive)
{
     while(*threadActive){
         auto start = chrono::high_resolution_clock::now();
         auto frame = frameManager->GetFree();
         if(frame != nullptr){
             DivideFrame(divider, frame, resultManager, timeManager, settings);
             frameManager->Clean(frame);
         }
         auto duration = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start);
         if(duration < settings->GetCaptureRate())
             this_thread::sleep_for(settings->GetCaptureRate() - duration);
     }
}

void AC::DivisionThreader::DivideFrame(FrameDivider *divider, FrameWrapper *frame, ResultManager *resultManager, TimeManager *timeManager, Settings *settings)
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

        // Add smooth frame to time stack if recursive
        if(settings->GetIsRecursiveSmoothing()){
            TimeSmoothing(&top, &bottom, &left, &right, timeManager, timeSmoothing);
            timeManager->Queue(top, bottom, left, right);
        }
        else{
            timeManager->Queue(top, bottom, left, right);
            TimeSmoothing(&top, &bottom, &left, &right, timeManager, timeSmoothing);
        }

        resultManager->Queue(top,bottom,left,right);
}

void AC::DivisionThreader::TimeSmoothing(vector<QColor> *top, vector<QColor> *bottom, vector<QColor> *left, vector<QColor> *right, TimeManager *timeManager, int timeSmoothing)
{
    ResultWrapper* pastframe;
    vector<WeightedAverageColor> topAverage;
    vector<WeightedAverageColor> bottomAverage;
    vector<WeightedAverageColor> leftAverage;
    vector<WeightedAverageColor> rightAverage;

    auto freeSize = timeManager->GetFreeSize();

    while((pastframe = timeManager->GetFree()) != nullptr){
        CalculateTimeSmoothing(&topAverage, pastframe->GetTopBegin(), pastframe->GetTopSize());
        CalculateTimeSmoothing(&bottomAverage, pastframe->GetBottomBegin(), pastframe->GetBottomSize());
        CalculateTimeSmoothing(&leftAverage, pastframe->GetLeftBegin(), pastframe->GetLeftSize());
        CalculateTimeSmoothing(&rightAverage, pastframe->GetRightBegin(), pastframe->GetRightSize());
        timeManager->Clean(pastframe);
    }

    timeManager->Rotate(freeSize);
    if(timeSmoothing == freeSize){
        timeManager->Clean(timeManager->GetFree());
    }

    ApplyTimeSmoothing(&topAverage, top->begin(), top->size());
    ApplyTimeSmoothing(&bottomAverage, bottom->begin(), bottom->size());
    ApplyTimeSmoothing(&leftAverage, left->begin(), left->size());
    ApplyTimeSmoothing(&rightAverage, right->begin(), right->size());
}

void AC::DivisionThreader::CalculateTimeSmoothing(vector<WeightedAverageColor> *average, vector<QColor>::iterator result, unsigned long size)
{
    for(unsigned long pos = 0; pos < size; pos++){
        if(average->size() != size){
            average->push_back(WeightedAverageColor());
        }
        average->at(pos).AddToAverage(&result[pos]);
    }
}

void AC::DivisionThreader::ApplyTimeSmoothing(vector<WeightedAverageColor> *average, vector<QColor>::iterator result, unsigned long size){
    if(average->size() == 0 || size != average->size())
        return;

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
