#include "buffers.h"

AC::FrameManager::FrameManager(){
    _logType = Statistics::StatisticType::Frame;
}

bool AC::FrameManager::Queue(const SL::Screen_Capture::Image &img){
    try {
        auto wrapper = this->Buffer::GetUsed();
        if(wrapper != nullptr){
            wrapper->CopyImageToBuffer(img);
            this->Buffer::Push(wrapper);
        }
        return true;
    }  catch (const exception& ex) {
        return false;
    }
}

AC::ResultManager::ResultManager(){
    _logType = Statistics::StatisticType::Result;
}

bool AC::ResultManager::Queue(
        vector<QColor> topColors,
        vector<QColor> bottomColors,
        vector<QColor> leftColors,
        vector<QColor> rightColors){
    try {
        auto wrapper = this->Buffer::GetUsed();
        if(wrapper != nullptr){
            wrapper->CopyColorsToWrapper(&topColors, &bottomColors, &leftColors, &rightColors);
            this->Buffer::Push(wrapper);
        }
        return true;
    }  catch (const exception& ex) {
        return false;
    }
}
