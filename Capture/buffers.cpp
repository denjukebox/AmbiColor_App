#include "buffers.h"

bool AC::FrameManager::Queue(const SL::Screen_Capture::Image &img, const Screen_Capture::Monitor &monitor){
    try {
        auto wrapper = this->Buffer::GetUsed();
        if(wrapper != nullptr){
            wrapper->CopyImageToBuffer(img, monitor);
            this->Buffer::Push(wrapper);
        }
        return true;
    }  catch (const exception& ex) {
        return false;
    }
}

bool AC::ResultManager::Queue(
        vector<QColor> topColors,
        vector<QColor> bottomColors,
        vector<QColor> leftColors,
        vector<QColor> rightColors){
    try {
        auto wrapper = GetUsed();
        if(wrapper != nullptr){
            wrapper->CopyColorsToWrapper(&topColors, &bottomColors, &leftColors, &rightColors);
            this->Buffer::Push(wrapper);
        }
        return true;
    }  catch (const exception& ex) {
        return false;
    }
}

bool AC::TimeManager::Queue(
        vector<QColor> topColors,
        vector<QColor> bottomColors,
        vector<QColor> leftColors,
        vector<QColor> rightColors){
    try {
        auto wrapper = GetUsed();
        if(wrapper != nullptr){
            wrapper->CopyColorsToWrapper(&topColors, &bottomColors, &leftColors, &rightColors);
            this->Buffer::Push(wrapper);
        }
        return true;
    }  catch (const exception& ex) {
        return false;
    }
}


void AC::TimeManager::Rotate(unsigned long distance){
    _free.swap(_used);
    if(_depth == distance){
        Clean(GetFree());
    }
    else
    {
        for(unsigned long pos = 0; pos < distance; pos++){
            Clean(GetFree());
        }
    }
}
