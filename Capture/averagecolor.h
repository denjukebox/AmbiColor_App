#ifndef AVERAGECOLOR_H
#define AVERAGECOLOR_H
#include <vector>
#include <queue>
#include <QColor>
#include <ScreenCapture.h>

using namespace std;
using namespace SL;

namespace AC {

    template<class InputT>
    class AverageColor
    {

    public:
        AverageColor(){
        }

        ~AverageColor(){
        }

        void AddStackToAverage(vector<InputT>* stack){
            _size += stack->size();
            for(long pos = 0; pos < stack->size(); pos++)
            {
                AddToAverage(&stack->at(pos));
            }
        }

        virtual void AddToAverage(InputT *value)
        {

        }

        virtual QColor GetAverage()
        {

        }

        protected:
        unsigned long long int _r = 0;
        unsigned long long int _g = 0;
        unsigned long long int _b = 0;
        unsigned long int _size = 0;
    };

    class FrameAverageColor : public AverageColor<Screen_Capture::ImageBGRA>
    {

    public:
        FrameAverageColor();

        void AddToAverage(Screen_Capture::ImageBGRA *value);
        QColor GetAverage();
    };


    class WheightedAverageColor : public AverageColor<QColor>
    {

    public:
        WheightedAverageColor(unsigned long int expectedSize);

        void AddToAverage(QColor *value);
        QColor GetAverage();

    private:
        unsigned long int _expectedSize;
        unsigned long long int _weight;
    };
}

#endif // AVERAGECOLOR_H
