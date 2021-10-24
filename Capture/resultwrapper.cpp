#include "resultwrapper.h"

AC::ResultWrapper::ResultWrapper()
{
}

void AC::ResultWrapper::CopyColorsToWrapper(
        vector<QColor> *topColors,
        vector<QColor> *bottomColors,
        vector<QColor> *leftColors,
        vector<QColor> *rightColors){
    _topColors = vector<QColor>(topColors->begin(), topColors->end());
    _bottomColors = vector<QColor>(bottomColors->begin(), bottomColors->end());
    _leftColors = vector<QColor>(leftColors->begin(), leftColors->end());
    _rightColors = vector<QColor>(rightColors->begin(), rightColors->end());
}

vector<QColor>::iterator AC::ResultWrapper::GetBegin(Direction direction){
    switch (direction) {
        case Direction::Top:
            return _topColors.begin();
        case Direction::Bottom:
            return _bottomColors.begin();
        case Direction::Left:
            return _leftColors.begin();
        case Direction::Right:
            return _rightColors.begin();
    }
}

unsigned long AC::ResultWrapper::GetSize(Direction direction){
    switch (direction) {
        case Direction::Top:
            return _topColors.size();
        case Direction::Bottom:
            return _bottomColors.size();
        case Direction::Left:
            return _leftColors.size();
        case Direction::Right:
            return _rightColors.size();
    }
}
