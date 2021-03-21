#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <iostream>
#include <string>
#include <typeinfo>
#include <typeindex>
#include "Capture/divisionthreader.h"
#include "Capture/framegrabber.h"
#include "Connector/teensylightconnector.h"

using namespace std;

namespace AC {
    class Logger
    {
    public:
        Logger();

        static void WriteActivity(const std::type_info& caller, string text);
        static void WriteError(const std::type_info& caller, string text);
        static void WriteDebug(const std::type_info& caller, string text);

    private:
        static void WriteLine(const std::type_info& caller, string text);
        static void WriteLine(string caller, string text);
    };
}
#endif // LOGGER_H
