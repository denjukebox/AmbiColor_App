#ifndef AVERAGECOLOR_H
#define AVERAGECOLOR_H
#include <vector>
#include <vector>
#include <QColor>
#include <ScreenCapture.h>

using namespace std;
using namespace SL;

namespace AC {
    class AverageColor
    {

    public:
        AverageColor();

        QColor GetQColor();
        void AddToAverage(vector<Screen_Capture::ImageBGRA>::iterator start, unsigned int size);

    private:
        unsigned long _sampleSize = 0;

        unsigned long _r = 0;
        unsigned long _g = 0;
        unsigned long _b = 0;
    };
}

#endif // AVERAGECOLOR_H
