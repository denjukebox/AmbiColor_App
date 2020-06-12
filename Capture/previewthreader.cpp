#include "previewthreader.h"

AC::PreviewThreader::PreviewThreader()
{
}

void AC::PreviewThreader::StartThread(){
    if(_threadActive)
        return;

    cout << "PREVIEWTHREADER:Started" << endl;
    _threadActive = true;
    _processThread = thread(DivideFrame, &_divider,
                                _manager, _settings,
                                &_threadActive);
}

void AC::PreviewThreader::StopThread(){
    if(!_threadActive)
        return;

    cout << "PREVIEWTHREADER:Stopped" << endl;
    _threadActive = false;
    _processThread.join();
}

void AC::PreviewThreader::DivideFrame(FrameDivider *divider, BufferManager *manager, Settings *settings, bool *threadActive)
{
    FrameWrapper* frame;
    while(*threadActive){
        auto start = chrono::high_resolution_clock::now();
        frame = manager->GetFreeFrame();
        if(frame != nullptr){
            Statistics::Instance().NextConsumed(Statistics::StatisticType::Frame);
            auto width = settings->GetWidth();
            auto height = settings->GetHeight();
            auto depth = settings->GetDepth();
            auto ratio =  settings->GetContentRatio();

            manager->QueueResult(CalculateTop(divider, frame, width, depth, ratio),
                                  CalculateBottom(divider, frame, width, depth, ratio),
                                  CalculateLeft(divider, frame, height, depth, ratio),
                                  CalculateRight(divider, frame, height, depth, ratio));

            Statistics::Instance().NextQueued(Statistics::StatisticType::Result);
            manager->CleanFrame(frame);
        }
        auto duration = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start);
        if(duration < settings->GetPreviewRate())
            this_thread::sleep_for(settings->GetPreviewRate() - duration);
    }
}


vector<QColor> AC::PreviewThreader::CalculateTop(FrameDivider *divider, FrameWrapper *frame, unsigned int blocksWidth, unsigned int depth, double contentRatio)
{

    auto verticalOffset = divider->CalculateVerticalOffset(frame->GetFrameWidth(), frame->GetFrameHeight(), contentRatio);
    return divider->CalculateHorizontalRow(frame->GetFrameBegin(),
                                blocksWidth,
                                frame->GetFrameWidth(),
                                verticalOffset,
                                depth,
                                divider->PixelsPerBlockWith(frame->GetFrameWidth(), blocksWidth));
}

vector<QColor> AC::PreviewThreader::CalculateBottom(FrameDivider *divider, FrameWrapper *frame, unsigned int blocksWidth, unsigned int depth, double contentRatio)
{
    auto verticalOffset = frame->GetFrameHeight() - depth - divider->CalculateVerticalOffset(frame->GetFrameWidth(), frame->GetFrameHeight(), contentRatio);
    return divider->CalculateHorizontalRow(frame->GetFrameBegin(),
                                blocksWidth,
                                frame->GetFrameWidth(),
                                verticalOffset,
                                depth,
                                divider->PixelsPerBlockWith(frame->GetFrameWidth(), blocksWidth));
}

vector<QColor> AC::PreviewThreader::CalculateLeft(FrameDivider *divider, FrameWrapper *frame, unsigned int blocksHeight, unsigned int depth, double contentRatio)
{
    auto horizontalOffset = divider->CalculateHorizontalOffset(frame->GetFrameWidth(), frame->GetFrameHeight(), contentRatio);
    return divider->CalculateVerticalRow(frame->GetFrameBegin(),
                                blocksHeight,
                                frame->GetFrameWidth(),
                                frame->GetFrameHeight(),
                                horizontalOffset,
                                depth,
                                divider->PixelsPerBlockHeight(frame->GetFrameHeight(), blocksHeight));
}

vector<QColor> AC::PreviewThreader::CalculateRight(FrameDivider *divider, FrameWrapper *frame, unsigned int blocksHeight, unsigned int depth, double contentRatio)
{
    auto horizontalOffset = frame->GetFrameWidth() - depth - divider->CalculateHorizontalOffset(frame->GetFrameWidth(), frame->GetFrameHeight(), contentRatio);
    return divider->CalculateVerticalRow(frame->GetFrameBegin(),
                                blocksHeight,
                                frame->GetFrameWidth(),
                                frame->GetFrameHeight(),
                                horizontalOffset,
                                depth,
                                divider->PixelsPerBlockHeight(frame->GetFrameHeight(), blocksHeight));
}
