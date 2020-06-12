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
            return GetTopBegin();
        case Direction::Bottom:
            return GetBottomBegin();
        case Direction::Left:
            return GetLeftBegin();
        case Direction::Right:
            return GetRightBegin();
    }
}

unsigned long AC::ResultWrapper::GetSize(Direction direction){
    switch (direction) {
        case Direction::Top:
            return GetTopSize();
        case Direction::Bottom:
            return GetBottomSize();
        case Direction::Left:
            return GetLeftSize();
        case Direction::Right:
            return GetRightSize();
    }
}

vector<QColor>::iterator AC::ResultWrapper::GetTopBegin(){
    return _topColors.begin();
}

unsigned long AC::ResultWrapper::GetTopSize(){
    return _topColors.size();
}

vector<QColor>::iterator AC::ResultWrapper::GetBottomBegin(){
    return _bottomColors.begin();
}

unsigned long AC::ResultWrapper::GetBottomSize(){
    return _bottomColors.size();
}

vector<QColor>::iterator AC::ResultWrapper::GetLeftBegin(){
    return _leftColors.begin();
}

unsigned long AC::ResultWrapper::GetLeftSize(){
    return _leftColors.size();
}

vector<QColor>::iterator AC::ResultWrapper::GetRightBegin(){
    return _rightColors.begin();
}

unsigned long AC::ResultWrapper::GetRightSize(){
    return _rightColors.size();
}
