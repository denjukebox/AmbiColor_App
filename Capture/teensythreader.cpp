#include "teensythreader.h"

//AC::TeensyThreader::TeensyThreader(TeensyLightConnector* connector) :
//_connector(connector)
//{
//}

//void AC::TeensyThreader::StartBroadcast(){
//    if(_threadActive || !_connector->IsConnected())
//        return;

//    cout << "TEENSYTHREADER:Started" << endl;

//    _threadActive = true;
//    _processThread = thread(DivideColors, &_divider,
//                            _connector, _manager,
//                            _settings, &_threadActive);
//}

//void AC::TeensyThreader::StopBroadcast(){
//    if(!_threadActive)
//        return;

//    cout << "TEENSYTHREADER:Stopped" << endl;
//    _threadActive = false;
//    _processThread.join();
//}

//bool AC::TeensyThreader::IsRunning(){
//    return _threadActive;
//}

//void AC::TeensyThreader::DivideColors(FrameDivider *divider, TeensyLightConnector *connector, BufferManager *manager, Settings *settings, bool *threadActive)
//{
//    auto order = settings->GetOrder();
//    auto tempBuffer = vector<unsigned char>();
//    while(*threadActive){
//        auto colorBuffer = vector<unsigned char>();
//        auto start = chrono::high_resolution_clock::now();
//        auto wrapper = manager->GetFreeFrame();
//        if(wrapper != nullptr && connector->IsConnected()){
//            //Log Consumed Frame
//            Statistics::Instance().NextConsumed(Statistics::StatisticType::Frame);

//            auto blocksWidth = settings->GetWidth();
//            auto blocksHeight = settings->GetHeight();
//            auto depth = settings->GetDepth();
//            auto ratio = settings->GetContentRatio();

//            auto pixelsPerBlockWidth = divider->PixelsPerBlockWith(wrapper->GetFrameWidth(), blocksWidth);
//            auto verticalOffset = divider->CalculateVerticalOffset(wrapper->GetFrameWidth(), wrapper->GetFrameHeight(), ratio);

//            auto pixelsPerBlockHeight = divider->PixelsPerBlockHeight(wrapper->GetFrameHeight(), blocksHeight);
//            auto horizontalOffset = divider->CalculateHorizontalOffset(wrapper->GetFrameWidth(), wrapper->GetFrameHeight(), ratio);

//            auto resultFrame = new ResultWrapper();

////            foreach (auto o, order) {
////                 switch (o) {
////                    case Direction::Top:
////                    tempBuffer = CalculateHorizontalRow(divider,
////                                                wrapper->GetFrameBegin(),
////                                                settings,
////                                                blocksWidth,
////                                                wrapper->GetFrameWidth(),
////                                                verticalOffset,
////                                                depth, pixelsPerBlockWidth, true);
////                    break;
////                    case Direction::Bottom:
////                    tempBuffer = CalculateHorizontalRow(divider,
////                                                wrapper->GetFrameBegin(),
////                                                settings,
////                                                blocksWidth,
////                                                wrapper->GetFrameWidth(),
////                                                wrapper->GetFrameHeight() - depth - verticalOffset,
////                                                depth, pixelsPerBlockWidth, true);
////                    break;
////                    case Direction::Left:
////                    tempBuffer = CalculateVerticalRow(divider,
////                                                wrapper->GetFrameBegin(),
////                                                settings,
////                                                blocksHeight,
////                                                wrapper->GetFrameWidth(),
////                                                wrapper->GetFrameHeight(),
////                                                horizontalOffset,
////                                                depth, pixelsPerBlockHeight, true);
////                    break;
////                    case Direction::Right:
////                    tempBuffer = CalculateVerticalRow(divider,
////                                                wrapper->GetFrameBegin(),
////                                                settings,
////                                                blocksHeight,
////                                                wrapper->GetFrameWidth(),
////                                                wrapper->GetFrameHeight(),
////                                                wrapper->GetFrameWidth() - depth - horizontalOffset,
////                                                depth, pixelsPerBlockHeight, true);
////                    break;
////                 }

////                 colorBuffer.insert(colorBuffer.end(), tempBuffer.begin(), tempBuffer.end());
////            }
////            if(!connector->PushOnSendQueue('F', colorBuffer.begin(), colorBuffer.end()))
////                cout << "TEENSYTHREADER:Failed sending frame" << endl;

//            std::future<vector<QColor>> topAsync = std::async(std::launch::async,
//                                                                CalculateHorizontalRow,
//                                                                divider,
//                                                                wrapper->GetFrameBegin(),
//                                                                settings,
//                                                                blocksWidth,
//                                                                wrapper->GetFrameWidth(),
//                                                                verticalOffset,
//                                                                depth, pixelsPerBlockWidth, true);
//            manager->CleanFrame(wrapper);
//            Statistics::Instance().NextQueued(Statistics::StatisticType::Teensy);
//        }
//        auto duration = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start);
//        if(duration < settings->GetBroadcastRate())
//            this_thread::sleep_for(settings->GetBroadcastRate() - duration);
//        else
//            cout << "TEENSYTHREADER:TOSLOW" << duration.count() << endl;
//    }
//}

//vector<QColor> AC::TeensyThreader::CalculateHorizontalRow(FrameDivider *divider, vector<Screen_Capture::ImageBGRA>::iterator imgRef, Settings *settings, int blocksWidth, int imageWidth, int offsetHeight, int depth, int pixelsPerBlock, bool inverted)
//{
//    vector<QColor> buffer;
//    for(auto blockPos = 0; blockPos < blocksWidth; blockPos++){
//        buffer.insert(buffer.end(), divider->CalculateHorizontalPixel(imgRef,
//                                                  offsetHeight,
//                                                  inverted ? (blocksWidth - blockPos)
//                                                           : blockPos,
//                                                  imageWidth,
//                                                  depth,
//                                                  pixelsPerBlock));
//    }

//    return buffer;
//}

//vector<QColor> AC::TeensyThreader::CalculateVerticalRow(FrameDivider *divider, vector<Screen_Capture::ImageBGRA>::iterator imgRef, Settings *settings, int blocksHeight, int imageWidth, int imageHeight, int offsetWidth, int depth, int pixelsPerBlock, bool inverted)
//{
//    vector<QColor> buffer;
//    for(auto blockPos = 0; blockPos < blocksHeight; blockPos++){
//        buffer.insert(buffer.end(), divider->CalculateVerticalPixel(imgRef,
//                                                  offsetWidth,
//                                                  inverted ? (blocksHeight - blockPos)
//                                                           : blockPos,
//                                                  imageWidth,
//                                                  imageHeight,
//                                                  depth,
//                                                  pixelsPerBlock));
//    }

//    return buffer;
//}
