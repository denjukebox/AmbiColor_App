#include "divisionthreader.h"

AC::DivisionThreader::DivisionThreader()
{
}

void AC::DivisionThreader::Start(){
    if(_threadActive)
        return;

    _threadActive = true;
    _processThread = thread(Thread, &_divider, _manager, _settings, &_threadActive);
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

void AC::DivisionThreader::Thread(FrameDivider *divider, BufferManager *manager, Settings *settings, bool *threadActive)
{
     while(*threadActive){
         auto frame = manager->GetFreeFrame();
         auto start = chrono::high_resolution_clock::now();
         if(frame != nullptr)
            DivideFrame(divider, frame, manager, settings);
         manager->CleanFrame(frame);
         auto duration = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start);
         if(duration < settings->GetCaptureRate())
             this_thread::sleep_for(settings->GetCaptureRate() - duration);
     }
}

void AC::DivisionThreader::DivideFrame(FrameDivider *divider, FrameWrapper *frame, BufferManager *manager, Settings *settings)
{
        auto width = settings->GetWidth();
        auto height = settings->GetHeight();
        auto depth = settings->GetDepth();
        auto ratio =  settings->GetContentRatio();

        auto asyncTop = async(launch::async, CalculateTop, divider, frame, width, depth, ratio);
        auto asyncBottom = async(launch::async, CalculateBottom, divider, frame, width, depth, ratio);
        auto asyncLeft = async(launch::async, CalculateLeft, divider, frame, height, depth, ratio);
        auto asyncRight = async(launch::async, CalculateRight, divider, frame, height, depth, ratio);
        manager->QueueResult(asyncTop.get(), asyncBottom.get(), asyncLeft.get(), asyncRight.get());
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
