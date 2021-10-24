#include "buffers.h"

bool AC::FrameManager::Queue(const SL::Screen_Capture::Image &img, const Screen_Capture::Monitor &monitor){
    try {
        auto wrapper = GetUsed();
        if(wrapper != nullptr){
            wrapper->CopyImageToBuffer(img, monitor);
            Push(wrapper);
        }
        return true;
    }  catch (const exception& ex) {
        return false;
    }
}

bool AC::ResultManager::Queue(
        vector<QColor> *topColors,
        vector<QColor> *bottomColors,
        vector<QColor> *leftColors,
        vector<QColor> *rightColors){
    try {
        auto wrapper = GetUsed();
        if(wrapper != nullptr){
            wrapper->CopyColorsToWrapper(topColors, bottomColors, leftColors, rightColors);
            Push(wrapper);
        }
        return true;
    }  catch (const exception& ex) {
        return false;
    }
}

bool AC::TimeManager::Queue(
        vector<QColor> *topColors,
        vector<QColor> *bottomColors,
        vector<QColor> *leftColors,
        vector<QColor> *rightColors){
    try {
        auto wrapper = GetUsed();
        if(wrapper != nullptr){
            wrapper->CopyColorsToWrapper(topColors, bottomColors, leftColors, rightColors);
            Push(wrapper);
        }
        return true;
    }  catch (const exception& ex) {
        return false;
    }
}


void AC::TimeManager::Rotate(){
    _free.swap(_used);
}
