#ifndef STATISTICS_H
#define STATISTICS_H

#include <thread>
#include <chrono>
#include <queue>
#include <mutex>
#include <map>
#include <numeric>

using namespace std;

namespace AC {
    class Statistics
    {
    public:
        Statistics();

        static Statistics &Instance() {static Statistics buffer; return buffer;}

        enum class StatisticType { Frame, Result, Teensy };

        void NextQueued(StatisticType type);
        void NextConsumed(StatisticType type);

        chrono::milliseconds GetQueueTime(StatisticType type);
        chrono::milliseconds GetConsumeTime(StatisticType type);

    private:
        static void Next(map<StatisticType, queue<chrono::high_resolution_clock::time_point>> *stack, map<StatisticType, mutex*> *lockStack, StatisticType type);
        static chrono::milliseconds GetTime(map<StatisticType, queue<chrono::high_resolution_clock::time_point>> *stack, map<StatisticType, mutex*> *lockStack, StatisticType type);

        const static int BUFFER_STACK_MAX_COUNT = 10;
        map<StatisticType, mutex*> _lockStack;
        map<StatisticType, queue<chrono::high_resolution_clock::time_point>> _queueStack;
        map<StatisticType, queue<chrono::high_resolution_clock::time_point>> _consumeStack;
    };
}

#endif // STATISTICS_H
