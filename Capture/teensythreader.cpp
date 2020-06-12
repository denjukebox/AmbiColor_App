#include "teensythreader.h"

AC::TeensyThreader::TeensyThreader(TeensyLightConnector* connector) :
_connector(connector)
{
}

void AC::TeensyThreader::StartBroadcast(){
    if(_threadActive )//|| !_connector->IsConnected()
        return;

    cout << "TEENSYTHREADER:Started" << endl;

    _threadActive = true;
    _processThread = thread(DivideColors, &_divider,
                            _connector, _manager,
                            _settings, &_threadActive);
}

void AC::TeensyThreader::StopBroadcast(){
    if(!_threadActive)
        return;

    cout << "TEENSYTHREADER:Stopped" << endl;
    _threadActive = false;
    _processThread.join();
}

bool AC::TeensyThreader::IsRunning(){
    return _threadActive;
}

void AC::TeensyThreader::DivideColors(FrameDivider *divider, TeensyLightConnector *connector, BufferManager *manager, Settings *settings, bool *threadActive)
{
    auto order = settings->GetOrder();
    auto tempBuffer = vector<unsigned char>();
    while(*threadActive){
        auto colorBuffer = vector<unsigned char>();
        auto start = chrono::high_resolution_clock::now();
        auto wrapper = manager->GetFreeFrame();
        if(wrapper != nullptr && connector->IsConnected()){
            Statistics::Instance().NextConsumed(Statistics::StatisticType::Frame);

            auto blocksWidth = settings->GetWidth();
            auto blocksHeight = settings->GetHeight();
            auto depth = settings->GetDepth();
            auto ratio = settings->GetContentRatio();

            auto pixelsPerBlockWidth = divider->PixelsPerBlockWith(wrapper->GetFrameWidth(), blocksWidth);
            auto verticalOffset = divider->CalculateVerticalOffset(wrapper->GetFrameWidth(), wrapper->GetFrameHeight(), ratio);

            auto pixelsPerBlockHeight = divider->PixelsPerBlockHeight(wrapper->GetFrameHeight(), blocksHeight);
            auto horizontalOffset = divider->CalculateHorizontalOffset(wrapper->GetFrameWidth(), wrapper->GetFrameHeight(), ratio);

            foreach (auto o, order) {
                 switch (o) {
                    case Direction::Top:
                    tempBuffer = CalculateHorizontalRow(divider,
                                                wrapper->GetFrameBegin(),
                                                settings,
                                                blocksWidth,
                                                wrapper->GetFrameWidth(),
                                                verticalOffset,
                                                depth, pixelsPerBlockWidth, true);
                    break;
                    case Direction::Bottom:
                    tempBuffer = CalculateHorizontalRow(divider,
                                                wrapper->GetFrameBegin(),
                                                settings,
                                                blocksWidth,
                                                wrapper->GetFrameWidth(),
                                                wrapper->GetFrameHeight() - depth - verticalOffset,
                                                depth, pixelsPerBlockWidth, true);
                    break;
                    case Direction::Left:
                    tempBuffer = CalculateVerticalRow(divider,
                                                wrapper->GetFrameBegin(),
                                                settings,
                                                blocksHeight,
                                                wrapper->GetFrameWidth(),
                                                wrapper->GetFrameHeight(),
                                                horizontalOffset,
                                                depth, pixelsPerBlockHeight, true);
                    break;
                    case Direction::Right:
                    tempBuffer = CalculateVerticalRow(divider,
                                                wrapper->GetFrameBegin(),
                                                settings,
                                                blocksHeight,
                                                wrapper->GetFrameWidth(),
                                                wrapper->GetFrameHeight(),
                                                wrapper->GetFrameWidth() - depth - horizontalOffset,
                                                depth, pixelsPerBlockHeight, true);
                    break;
                 }

                 colorBuffer.insert(colorBuffer.end(), tempBuffer.begin(), tempBuffer.end());
            }
            if(!connector->PushOnSendQueue('F', colorBuffer.begin(), colorBuffer.end()))
                cout << "TEENSYTHREADER:Failed sending frame" << endl;

            manager->CleanFrame(wrapper);
            Statistics::Instance().NextQueued(Statistics::StatisticType::Teensy);
        }
        auto duration = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start);
        if(duration < settings->GetBroadcastRate())
            this_thread::sleep_for(settings->GetBroadcastRate() - duration);
        else
            cout << "TEENSYTHREADER:TOSLOW" << duration.count() << endl;
    }
}

vector<unsigned char> AC::TeensyThreader::CalculateHorizontalRow(FrameDivider *divider, vector<Screen_Capture::ImageBGRA>::iterator imgRef, Settings *settings, int blocksWidth, int imageWidth, int offsetHeight, int depth, int pixelsPerBlock, bool inverted)
{
    vector<unsigned char> buffer;
    for(auto blockPos = 0; blockPos < blocksWidth; blockPos++){
        auto result = divider->CalculateHorizontalPixel(imgRef,
                                                  offsetHeight,
                                                  inverted ? (blocksWidth - blockPos)
                                                           : blockPos,
                                                  imageWidth,
                                                  depth,
                                                  pixelsPerBlock);
        ApplyCorrection(&result, settings->GetBrightnessFactor(), settings->GetColorCorrectionRedFactor(), settings->GetColorCorrectionGreenFactor(), settings->GetColorCorrectionBlueFactor());
        PushColorOnBuffer(&buffer, &result, settings->GetColorOrder());
    }

    return buffer;
}

vector<unsigned char> AC::TeensyThreader::CalculateVerticalRow(FrameDivider *divider, vector<Screen_Capture::ImageBGRA>::iterator imgRef, Settings *settings, int blocksHeight, int imageWidth, int imageHeight, int offsetWidth, int depth, int pixelsPerBlock, bool inverted)
{
    vector<unsigned char> buffer;
    for(auto blockPos = 0; blockPos < blocksHeight; blockPos++){
        auto result = divider->CalculateVerticalPixel(imgRef,
                                                  offsetWidth,
                                                  inverted ? (blocksHeight - blockPos)
                                                           : blockPos,
                                                  imageWidth,
                                                  imageHeight,
                                                  depth,
                                                  pixelsPerBlock);
        ApplyCorrection(&result, settings->GetBrightnessFactor(), settings->GetColorCorrectionRedFactor(), settings->GetColorCorrectionGreenFactor(), settings->GetColorCorrectionBlueFactor());
        PushColorOnBuffer(&buffer, &result, settings->GetColorOrder());
    }

    return buffer;
}

void AC::TeensyThreader::ApplyCorrection(QColor *color, double brightnessFactor, double redFactor, double greenFactor, double blueFactor){
    color->setRed(color->red() * redFactor * brightnessFactor);
    color->setGreen(color->green() * greenFactor * brightnessFactor);
    color->setBlue(color->blue() * blueFactor * brightnessFactor);
}

void AC::TeensyThreader::PushColorOnBuffer(vector<unsigned char> *buffer, QColor *color, ColorOrder order){
    switch (order) {
        case ColorOrder::RGB:
            buffer->push_back(color->red());
            buffer->push_back(color->green());
            buffer->push_back(color->blue());
            break;
        case ColorOrder::RBG:
            buffer->push_back(color->red());
            buffer->push_back(color->blue());
            buffer->push_back(color->green());
            break;
        case ColorOrder::GRB:
            buffer->push_back(color->green());
            buffer->push_back(color->red());
            buffer->push_back(color->blue());
            break;
        case ColorOrder::GBR:
            buffer->push_back(color->green());
            buffer->push_back(color->blue());
            buffer->push_back(color->red());
            break;

    }
}

//            foreach (auto o, order) {
//                auto offset = -1;
//                auto max = 0;
//                while(offset < max){
//                    switch (o) {
//                      case Direction::Top:
//                        if(offset == -1){
//                            offset = 0;
//                            max = settings->GetWidth();
//                        }
//                        colorBuff = CalculateHorizontal(divider, &offset,
//                                                        incompleteBuff.empty()
//                                                        ? settings->GetBufferSize()
//                                                        : settings->GetBufferSize() - (incompleteBuff.size() / 3),
//                                                        wrapper->GetFrameBegin(),
//                                                        width, wrapper->GetFrameWidth(),
//                                                        verticalOffset,
//                                                        depth, pixelsPerBlockWidth, true);
//                        break;
//                      case Direction::Bottom:
//                        if(offset == -1){
//                            offset = 0;
//                            max = settings->GetWidth();
//                        }
//                        colorBuff = CalculateHorizontal(divider, &offset,
//                                                        incompleteBuff.empty()
//                                                        ? settings->GetBufferSize()
//                                                        : settings->GetBufferSize() - (incompleteBuff.size() / 3),
//                                                        wrapper->GetFrameBegin(),
//                                                        width, wrapper->GetFrameWidth(),
//                                                        wrapper->GetFrameHeight() - depth - verticalOffset,
//                                                        depth, pixelsPerBlockWidth, true);
//                        break;
//                      case Direction::Left:
//                        if(offset == -1){
//                            offset = 0;
//                            max = settings->GetHeight();
//                        }
//                        colorBuff = CalculateVertical(divider, &offset,
//                                                      incompleteBuff.empty()
//                                                      ? settings->GetBufferSize()
//                                                      : settings->GetBufferSize() - (incompleteBuff.size() / 3),
//                                                      wrapper->GetFrameBegin(),
//                                                   height, wrapper->GetFrameWidth(), wrapper->GetFrameHeight(),
//                                                   horizontalOffset,
//                                                   depth, pixelsPerBlockHeight, true);
//                        break;
//                      case Direction::Right:
//                        if(offset == -1){
//                            offset = 0;
//                            max = settings->GetHeight();
//                        }
//                        colorBuff = CalculateVertical(divider, &offset,
//                                                      incompleteBuff.empty()
//                                                      ? settings->GetBufferSize()
//                                                      : settings->GetBufferSize() - (incompleteBuff.size() / 3),
//                                                      wrapper->GetFrameBegin(),
//                                                       height, wrapper->GetFrameWidth(), wrapper->GetFrameHeight(),
//                                                       wrapper->GetFrameWidth() - depth - horizontalOffset,
//                                                       depth, pixelsPerBlockHeight, true);
//                        break;
//                    }

//                    if(settings->GetBufferSize() != colorBuff.size() && incompleteBuff.empty() && o != order.back())
//                    {
//                        incompleteBuff = colorBuff;
//                    }
//                    else
//                    {
//                        if(!incompleteBuff.empty())
//                        {
//                            incompleteBuff.insert(incompleteBuff.end(), colorBuff.begin(), colorBuff.end());
//                            colorBuff = incompleteBuff;
//                            incompleteBuff.clear();
//                        }

//                        if(!connector->PushOnSendQueue('D', colorBuff.begin(), colorBuff.end()))
//                            cout << "Failed Sending" << endl;
//                    }
//                }
//            }
//            connector->PushOnSendQueue('F');
//            manager->CleanFrame(wrapper);
//            Statistics::Instance().NextQueued(Statistics::StatisticType::Teensy);

        //*threadActive = false;
