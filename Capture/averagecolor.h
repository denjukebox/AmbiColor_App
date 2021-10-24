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
            for(unsigned long pos = 0; pos < stack->size(); pos++)
            {
                AddToAverage(&stack->at(pos));
            }
        }

        virtual void AddToAverage(InputT *value){

        }

        virtual QColor GetAverage(){
            return  QColor();
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


    class WeightedAverageColor : public AverageColor<QColor>
    {

    public:
        WeightedAverageColor(unsigned long expected){
            _expected = expected;
        }

        void AddToAverage(QColor *value);
        QColor GetAverage();

    private:
        unsigned long long int _expected = 0;
    };
}

#endif // AVERAGECOLOR_H
