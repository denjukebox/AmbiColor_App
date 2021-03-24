#include "framegrabber.h"

AC::FrameGrabber::FrameGrabber(FrameManager *frameManager)
{
    _frameManager = frameManager;
    _monitor = Screen_Capture::GetMonitors().at(0);
}

void AC::FrameGrabber::StartCapture(){
    if(_isRunning)
        StopCapture();

    Logger::WriteActivity(typeid(this), "Started");
    _grabber = Screen_Capture::CreateCaptureConfiguration([&](){return vector<Screen_Capture::Monitor>{_monitor};})
    ->onNewFrame([&](const Screen_Capture::Image &img, const Screen_Capture::Monitor &monitor){
        ProcessCapturedFrame(_frameManager, img, monitor);
    })->start_capturing();

    _grabber->setFrameChangeInterval(_settings->GetCaptureRate());
    _isRunning = true;
}

void AC::FrameGrabber::StopCapture()
{
    if(_grabber != nullptr){
        _grabber->pause();
        _grabber = nullptr;
        Logger::WriteActivity(typeid(this), "Stopped");
    }
    _isRunning = false;
}

void AC::FrameGrabber::ProcessCapturedFrame(FrameManager *frameManager, const Screen_Capture::Image &img, const Screen_Capture::Monitor &monitor){
    if(frameManager->Queue(img))
        Statistics::Instance().NextQueued(Statistics::StatisticType::Frame);
}

void AC::FrameGrabber::SetMonitor(Screen_Capture::Monitor monitor){
    _monitor = monitor;    
    Logger::WriteActivity(typeid(this), "SetMonitor");
}

bool AC::FrameGrabber::IsRunning()
{
    return _isRunning;
}

AC::FrameGrabber::~FrameGrabber()
{
    StopCapture();
}
