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
        return wrapper;
    }  catch (const exception& ex) {
        return nullptr;
    }
}

int AC::BufferManager::GetPosition(FrameWrapper* frameRef){
    return distance(&_framesStack.front(), frameRef);
}

void AC::BufferManager::CleanFrame(FrameWrapper* frameRef){
    _usedFrames.push(frameRef);
}

void AC::BufferManager::ReuseFrame(FrameWrapper* frameRef){
    _freeFrames.push(frameRef);
}

void AC::BufferManager::QueueResult(
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
    }  catch (const exception& ex) {
    }
}

AC::ResultWrapper* AC::BufferManager::GetFreeResult(){
    try {
        if(_freeResults.empty())
            throw exception();
        lock_guard<mutex> guard(_resultLock);
        auto wrapper = _freeResults.front();
        _freeResults.pop();
        return wrapper;
    }  catch (const exception& ex) {
        return nullptr;
    }
}

void AC::BufferManager::CleanResult(ResultWrapper* resultRef){
    _usedResults.push(resultRef);
}


void AC::BufferManager::ReuseResult(ResultWrapper* resultRef){
    _freeResults.push(resultRef);
}

int AC::BufferManager::GetPosition(ResultWrapper* resultRef){
    return distance(&_resultsStack.front(), resultRef);
}
