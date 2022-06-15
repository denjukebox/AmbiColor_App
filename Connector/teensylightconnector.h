#ifndef TEENSYLIGHTCONNECTOR_H
#define TEENSYLIGHTCONNECTOR_H

// C library headers
#include <stdio.h>
#include <string.h>

// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

#include <QColor>
#include <Capture/averagecolor.h>
#include <Capture/buffers.h>
#include "Capture/enums.h"
#include <Views/configuration.h>
#include "Util/settings.h"
#include "Util/statistics.h"
#include "Util/logger.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

using namespace std;

namespace AC {
    class TeensyLightConnector
    {

    public:
        TeensyLightConnector(ResultManager *resultManager);
        ~TeensyLightConnector();

        bool StartBroadcast();
        bool StopBroadcast();

        bool OpenDevice();
        bool CloseDevice();

        bool ConfigureDevice();

        bool IsConnected();


        static const char CONTROL = '*';
        static const char COMMAND_CONFIG = 'C';
        static const char COMMAND_FRAME = 'F';

    private:

        static void Thread(int portHandle, ResultManager *resultManager, Settings *settings, bool *threadActive);
        static void ProccessResult(ResultWrapper *wrapper, unsigned char *buffer, int offset, Settings *settings);
        static bool PushToTeensy(int _portHandle, unsigned char *buffer, long size);

        static unsigned long PushColorsOnBuffer(unsigned char *buffer, unsigned long bufferOffset, vector<QColor>::iterator colors, unsigned long colorsLength, ColorOrder order);
        static void ApplyCorrection(vector<QColor>::iterator colors, unsigned long colorsLength, double brightnessFactor, double redFactor, double greenFactor, double blueFactor);

        static unsigned long PushIntOnBuffer(unsigned char *buffer, unsigned long offset, int num);
        static unsigned long PushOrderOnBuffer(unsigned char *buffer, unsigned long offset, Direction dir);
        static unsigned long PushCommandOnBuffer(unsigned char *buffer, char command);
        static unsigned long PushOnBuffer(unsigned char *buffer, unsigned long bufferOffset, char value);

        const char* SERIAL_DEV = "/dev/ttyACM0";

        Settings *_settings = &Settings::Instance();
        ResultManager *_resultManager;

        struct termios _teensyHandle;
        int _portHandle;
        bool _deviceConfigured = false;

        thread _processThread;
        bool _threadActive = false;

        const static int CONFIG_FRAME_SERIAL_RX_SIZE = 64;
        const static int COMMAND_BUFFER_OFFSET = 3;
    };
}
#endif // TEENSYLIGHTCONNECTOR_H
