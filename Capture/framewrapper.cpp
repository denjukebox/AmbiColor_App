#include "framewrapper.h"

AC::FrameWrapper::FrameWrapper()
{
}

void AC::FrameWrapper::CopyImageToBuffer(const Screen_Capture::Image &img, const Screen_Capture::Monitor &monitor){
    _monitor = monitor;
    _frameWidth = Screen_Capture::Width(img);
    _frameHeight = Screen_Capture::Height(img);
    _frameBuffer = vector<Screen_Capture::ImageBGRA>(StartSrc(img),  StartSrc(img) + (Width(img) * Height(img)));
}

vector<Screen_Capture::ImageBGRA>::iterator AC::FrameWrapper::GetFrameBegin()
{
    return _frameBuffer.begin();
}

unsigned int AC::FrameWrapper::GetFrameWidth()
{
    return _frameWidth;
}

unsigned int AC::FrameWrapper::GetFrameHeight()
{
    return _frameHeight;
}


Screen_Capture::Monitor AC::FrameWrapper::GetMonitor(){
    return _monitor;
}

