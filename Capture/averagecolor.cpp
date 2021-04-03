#include "averagecolor.h"

AC::FrameAverageColor::FrameAverageColor(){
}

void AC::FrameAverageColor::AddToAverage(Screen_Capture::ImageBGRA *color)
{
    _r += color->R;
    _g += color->G;
    _b += color->B;
    _size++;
}

QColor AC::FrameAverageColor::GetAverage()
{
    int r = _r / _size;
    int g = _g / _size;
    int b = _b / _size;
    return QColor(r, b, g);
}

AC::WeightedAverageColor::WeightedAverageColor(){
}

void AC::WeightedAverageColor::AddToAverage(QColor *color)
{
    _size++;
    _r += color->red() * _size;
    _g += color->green() * _size;
    _b += color->blue() * _size;
    _weight += _size;
}

QColor AC::WeightedAverageColor::GetAverage()
{
    int r = _r / _weight;
    int g = _g / _weight;
    int b = _b / _weight;
    return QColor(r, b, g);
}
