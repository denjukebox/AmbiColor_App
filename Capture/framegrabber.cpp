#include "framegrabber.h"

AC::FrameGrabber::FrameGrabber()
{
    _monitor = Screen_Capture::GetMonitors().at(0);
}

void AC::FrameGrabber::StartCapture(){
    if(_isRunning)
        StopCapture();

    cout << "FRAMEGRABBER:Started" << endl;
    _grabber = Screen_Capture::CreateCaptureConfiguration([&](){return vector<Screen_Capture::Monitor>{_monitor};})
    ->onNewFrame([&](const Screen_Capture::Image &img, const Screen_Capture::Monitor &monitor){
        ProcessCapturedFrame(_manager, img, monitor);
    })->start_capturing();

    _grabber->setFrameChangeInterval(_settings->GetCaptureRate());
    _isRunning = true;
}

void AC::FrameGrabber::StopCapture()
{
    if(_grabber != nullptr){
        _grabber->pause();
        _grabber = nullptr;
        cout << "FRAMEGRABBER:Stopped" << endl;
    }
    _isRunning = false;
}

void AC::FrameGrabber::ProcessCapturedFrame(BufferManager *manager, const Screen_Capture::Image &img, const Screen_Capture::Monitor &monitor){
    if(manager->QueueFrame(img))
        Statistics::Instance().NextQueued(Statistics::StatisticType::Frame);
}

void AC::FrameGrabber::SetMonitor(Screen_Capture::Monitor monitor){
    _monitor = monitor;    
    cout << "FRAMEGRABBER:SetMonitor" << monitor.Name << endl;
}

bool AC::FrameGrabber::IsRunning()
{
    return _isRunning;
}

AC::FrameGrabber::~FrameGrabber()
{
    StopCapture();
}
