#include "averagecolor.h"
AC::AverageColor::AverageColor(){
}

void AC::AverageColor::AddToAverage(vector<Screen_Capture::ImageBGRA>::iterator start, unsigned int size)
{

    _sampleSize += size;
    for(long pos = 0; pos < size; pos++)
    {
        _r += start[pos].R;
        _g += start[pos].G;
        _b += start[pos].B;
    }
}

QColor AC::AverageColor::GetQColor()
{
    //TODO POST PROCESSING
    return QColor(_r / _sampleSize, _g / _sampleSize, _b / _sampleSize);
}
