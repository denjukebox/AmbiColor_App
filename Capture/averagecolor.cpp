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
    return QColor(r, g, b);
}

void AC::WeightedAverageColor::AddToAverage(QColor *color)
{
    _size++;
    _r += color->red();
    _g += color->green();
    _b += color->blue();
}

QColor AC::WeightedAverageColor::GetAverage()
{
    int r = _r / _size;
    int g = _g / _size;
    int b = _b / _size;
    return QColor(r, g, b);
}
