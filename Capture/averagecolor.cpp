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

void AC::WeightedAverageColor::AddToAverage(QColor *color)
{
    _size++;
    auto factor = _expected / _size;
    _r += color->red() * factor;
    _g += color->green() * factor;
    _b += color->blue() * factor;
}

QColor AC::WeightedAverageColor::GetAverage()
{
    int r = _r / _expected;
    int g = _g / _expected;
    int b = _b / _expected;
    return QColor(r, b, g);
}
