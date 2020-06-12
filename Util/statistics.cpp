#include "statistics.h"

AC::Statistics::Statistics()
{
    _lockStack.emplace(StatisticType::Frame, new mutex());
    _lockStack.emplace(StatisticType::Teensy, new mutex());
    _lockStack.emplace(StatisticType::Result, new mutex());
}

void AC::Statistics::NextQueued(StatisticType type)
{
    Next(&_queueStack, &_lockStack, type);
}

void AC::Statistics::NextConsumed(StatisticType type)
{
    Next(&_consumeStack, &_lockStack, type);
}

chrono::milliseconds AC::Statistics::GetQueueTime(StatisticType type){
    return GetTime(&_queueStack, &_lockStack, type);
}

chrono::milliseconds AC::Statistics::GetConsumeTime(StatisticType type){
    return GetTime(&_consumeStack, &_lockStack, type);
}

void AC::Statistics::Next(map<StatisticType, queue<chrono::high_resolution_clock::time_point>> *stack, map<StatisticType, mutex*> *lockStack, StatisticType type)
{
    lock_guard<mutex> guard(*lockStack->at(type));
    auto ref = stack->find(type);
    if(ref == stack->end())
        ref = stack->emplace(type, queue<chrono::high_resolution_clock::time_point>()).first;

    ref->second.push(chrono::high_resolution_clock::now());
}


chrono::milliseconds AC::Statistics::GetTime(map<StatisticType, queue<chrono::high_resolution_clock::time_point>> *stack, map<StatisticType, mutex*> *lockStack, StatisticType type){
    lock_guard<mutex> guard(*lockStack->at(type));
    auto duration = chrono::milliseconds(0);
    auto ref = stack->find(type);
    if(ref != stack->end() && ref->second.size() > 2){
        auto size = ref->second.size();
        auto prevPoint = ref->second.front();
        while(prevPoint != ref->second.back()){
            ref->second.pop();
            auto nextPoint = ref->second.front();
            duration += chrono::duration_cast<chrono::milliseconds>(nextPoint - prevPoint);
            prevPoint = nextPoint;
        }
        duration = duration / size;
    }
    return duration;
}
