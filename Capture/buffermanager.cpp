#include "buffermanager.h"

AC::BufferManager::BufferManager()
{
    FillBuffers();
    SyncBufferUse();
}

void AC::BufferManager::SyncBufferUse(){
    for(auto pos = 0; pos < BUFFER_STACK_MAX_COUNT; pos++){
        _usedFrames.push(&_framesStack[pos]);
        _usedResults.push(&_resultsStack[pos]);
    }
}

void AC::BufferManager::FillBuffers(){
    for(auto pos = 0; pos < BUFFER_STACK_MAX_COUNT; pos++){
        _framesStack[pos] = FrameWrapper();
        _resultsStack[pos] = ResultWrapper();
    }
}

bool AC::BufferManager::QueueFrame(const SL::Screen_Capture::Image &img){
    try {
        if(_usedFrames.empty())
            throw exception();
        lock_guard<mutex> guard(_frameLock);
        auto wrapper = _usedFrames.front();
        _usedFrames.pop();
        wrapper->CopyImageToBuffer(img);
        _freeFrames.push(wrapper);
        Statistics::Instance().NextQueued(Statistics::StatisticType::Frame);
        return true;
    }  catch (const exception& ex) {
        return false;
    }
}

AC::FrameWrapper* AC::BufferManager::GetFreeFrame(){
    try {
        if(_freeFrames.empty())
            throw exception();
        lock_guard<mutex> guard(_frameLock);
        auto wrapper = _freeFrames.front();
        _freeFrames.pop();
        Statistics::Instance().NextConsumed(Statistics::StatisticType::Frame);
        return wrapper;
    }  catch (const exception& ex) {
        return nullptr;
    }
}

int AC::BufferManager::GetPosition(FrameWrapper* frameRef){
    return distance(&_framesStack.front(), frameRef);
}

bool AC::BufferManager::CleanFrame(FrameWrapper* frameRef){
    try {
        _usedFrames.push(frameRef);
        return true;
    }   catch (const exception& ex) {
        return false;
    }
}

bool AC::BufferManager::ReuseFrame(FrameWrapper* frameRef){
    try {
        _freeFrames.push(frameRef);
        return true;
    }   catch (const exception& ex) {
        return false;
    }
}

bool AC::BufferManager::QueueResult(
        vector<QColor> topColors,
        vector<QColor> bottomColors,
        vector<QColor> leftColors,
        vector<QColor> rightColors){
    try {

        if(_usedResults.empty())
            throw exception();
        lock_guard<mutex> guard(_resultLock);
        auto wrapper = _usedResults.front();
        _usedResults.pop();
        wrapper->CopyColorsToWrapper(&topColors, &bottomColors, &leftColors, &rightColors);
        _freeResults.push(wrapper);
        Statistics::Instance().NextQueued(Statistics::StatisticType::Result);
        return true;
    }  catch (const exception& ex) {
        return false;
    }
}

AC::ResultWrapper* AC::BufferManager::GetFreeResult(){
    try {
        if(_freeResults.empty())
            throw exception();
        lock_guard<mutex> guard(_resultLock);
        auto wrapper = _freeResults.front();
        _freeResults.pop();
        Statistics::Instance().NextConsumed(Statistics::StatisticType::Result);
        return wrapper;
    }  catch (const exception& ex) {
        return nullptr;
    }
}

bool AC::BufferManager::CleanResult(ResultWrapper* resultRef){
    try {
        _usedResults.push(resultRef);
        return true;
    }   catch (const exception& ex) {
        return false;
    }
}

bool AC::BufferManager::ReuseResult(ResultWrapper* resultRef){
    try {
        _freeResults.push(resultRef);
        return true;
    }   catch (const exception& ex) {
        return false;
    }
}

int AC::BufferManager::GetPosition(ResultWrapper* resultRef){
    return distance(&_resultsStack.front(), resultRef);
}
