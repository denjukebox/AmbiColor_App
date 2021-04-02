#include "settings.h"

AC::Settings::Settings()
{

}

//Capture

int AC::Settings::GetWidth()
{
    return _width;
}

void AC::Settings::SetWidth(int width)
{
    _width = width;
}

int AC::Settings::GetHeight()
{
    return _height;
}

void AC::Settings::SetHeight(int height)
{
    _height = height;
}

int AC::Settings::GetDepth()
{
    return _depth;
}

void AC::Settings::SetDepth(int depth)
{
    _depth = depth;
}

int AC::Settings::GetTimeSmoothing()
{
    return _timeSmoothing;
}

void AC::Settings::SetTimeSmoothing(int timeSmoothing)
{
    _timeSmoothing = timeSmoothing;
}

bool AC::Settings::GetIsRecursiveSmoothing()
{
    return _isRecurisveSmoothing;
}

void AC::Settings::SetIsRecursiveSmoothing(bool isRecursiveSmoothing)
{
    _isRecurisveSmoothing = isRecursiveSmoothing;
}

bool AC::Settings::GetIsDividedAsync()
{
    return _isDividedAsync;
}

void AC::Settings::SetIsDividedAsync(bool isDividedAsync)
{
    _isDividedAsync = isDividedAsync;
}

double AC::Settings::GetContentRatio()
{
    return _ratio;
}

void AC::Settings::SetContentRatio(double ratio)
{
    _ratio = ratio;
}

chrono::milliseconds AC::Settings::GetCaptureRate()
{
    return _captureRate;
}

void AC::Settings::SetCaptureRate(chrono::milliseconds rate)
{
    _captureRate = rate;
}

void AC::Settings::SetColorOrder(ColorOrder order)
{
    _colorOrder = order;
}

ColorOrder AC::Settings::GetColorOrder()
{
    return _colorOrder;
}

void AC::Settings::SetColorCorrection(LEDColorCorrection correction)
{
    _correction = correction;
}

LEDColorCorrection AC::Settings::GetColorCorrection()
{
    return _correction;
}

//Controller

int AC::Settings::GetBrightness(){
    return _brightness;
}

void AC::Settings::SetBrightness(int brightness)
{
    _brightness = brightness;
}

float AC::Settings::GetBrightnessFactor(){
    return (float)_brightness / 255;
}

float AC::Settings::GetColorCorrectionRedFactor(){
    return (float)(((size_t)_correction & 0xFF0000) >> 16) / 255;
}

float AC::Settings::GetColorCorrectionGreenFactor(){
    return (float)(((size_t)_correction & 0x00FF00) >> 8) / 255;
}

float AC::Settings::GetColorCorrectionBlueFactor(){
    return (float)((size_t)_correction & 0x0000FF) / 255;
}


int AC::Settings::GetNumberOfStrips(){
    return _numberOfStrips;
}

void AC::Settings::SetNumberOfStrips(int numberOfStrips)
{
    _numberOfStrips = numberOfStrips;
}

int AC::Settings::GetBufferSize(){
    return GetAmoutPixels() * 3; //3 bytes per color (match controller bytes per color)
}

int AC::Settings::GetAmoutPixels(){
    return (_height + _width) * 2;
}

vector<Direction> AC::Settings::GetOrder(){
    return _order;
}

void AC::Settings::SetOrder(vector<Direction> order)
{
    _order = order;
}

//Preview

chrono::milliseconds AC::Settings::GetPreviewRate()
{
    return _previewRate;
}

void AC::Settings::SetPreviewRate(chrono::milliseconds rate)
{
    _previewRate = rate;
}





