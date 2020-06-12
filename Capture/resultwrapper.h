#ifndef RESULTWRAPPER_H
#define RESULTWRAPPER_H

#include <QColor>
#include "enums.h"

using namespace std;

namespace AC {
    class ResultWrapper
    {

    public:
        explicit ResultWrapper();

        void CopyColorsToWrapper(
                vector<QColor> *topColors,
                vector<QColor> *bottomColors,
                vector<QColor> *leftColors,
                vector<QColor> *rightColors);

        vector<QColor>::iterator GetBegin(Direction direction);
        unsigned long GetSize(Direction direction);

        vector<QColor>::iterator GetTopBegin();
        unsigned long GetTopSize();

        vector<QColor>::iterator GetBottomBegin();
        unsigned long GetBottomSize();

        vector<QColor>::iterator GetLeftBegin();
        unsigned long GetLeftSize();

        vector<QColor>::iterator GetRightBegin();
        unsigned long GetRightSize();

    private:
        vector<QColor> _topColors = vector<QColor>();
        vector<QColor> _bottomColors = vector<QColor>();
        vector<QColor> _leftColors = vector<QColor>();
        vector<QColor> _rightColors = vector<QColor>();
    };
}

#endif // RESULTWRAPPER_H
