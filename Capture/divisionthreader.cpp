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

        auto asyncTop = async(launch::async, CalculateTop, divider, frame, width, depth, ratio);
        auto asyncBottom = async(launch::async, CalculateBottom, divider, frame, width, depth, ratio);
        auto asyncLeft = async(launch::async, CalculateLeft, divider, frame, height, depth, ratio);
        auto asyncRight = async(launch::async, CalculateRight, divider, frame, height, depth, ratio);

        auto top = asyncTop.get();// CalculateTop( divider, frame, width, depth, ratio);
        auto bottom = asyncBottom.get();// CalculateBottom( divider, frame, width, depth, ratio);
        auto left = asyncLeft.get();// CalculateLeft( divider, frame, height, depth, ratio);
        auto right = asyncRight.get();// CalculateRight( divider, frame, height, depth, ratio);

        ResultWrapper* pastframe;
        for(auto pos = 0; pos < 10; pos++ ){
            pastframe = timeManager->GetFree();
            if(pastframe == nullptr){
                timeManager->Push(timeManager->GetUsed());
            }
            else{
                AverageColors(&top, pastframe->GetTopBegin(), pastframe->GetTopSize());
                AverageColors(&bottom, pastframe->GetBottomBegin(), pastframe->GetBottomSize());
                AverageColors(&left, pastframe->GetLeftBegin(), pastframe->GetLeftSize());
                AverageColors(&right, pastframe->GetRightBegin(), pastframe->GetRightSize());
                timeManager->Clean(pastframe);
            }
        }

        timeManager->Queue(top,bottom,left,right);
        resultManager->Queue(top,bottom,left,right);
}

void AC::DivisionThreader::AverageColors(vector<QColor> *result1, vector<QColor>::iterator result2, unsigned long size){
    if(result1->size() != size)
        return;
    for(unsigned long pos = 0; pos < result1->size(); pos++){
        result1->at(pos).setRed((result1->at(pos).red() + result2[pos].red()) / 2);
        result1->at(pos).setGreen((result1->at(pos).green() + result2[pos].green()) / 2);
        result1->at(pos).setBlue((result1->at(pos).blue() + result2[pos].blue()) / 2);
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
