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
#include <Capture/buffermanager.h>
#include "Capture/enums.h"
#include <Views/configuration.h>
#include "Util/settings.h"
#include "Util/statistics.h"
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
        TeensyLightConnector();        
        ~TeensyLightConnector();

        bool OpenDevice();
        bool CloseDevice();

        bool ConfigureDevice();

        bool IsConnected();

        bool PushOnSendQueue(char command);
        bool PushOnSendQueue(char command, vector<unsigned char>::iterator start, vector<unsigned char>::iterator end);
        bool PushOnSendQueue(const vector<unsigned char> buffer);

    private:
        static void SendBuffer(int _portHandle, queue<vector<unsigned char>> *sendQueue,mutex* sendMutex, bool *threadActive);
        static bool PushToTeensy(int _portHandle, vector<unsigned char>::iterator start, vector<unsigned char>::iterator end);

        void PushIntOnBuffer(vector<unsigned char> *buffer, int num);

        int GetBufferSize();

        const char* SERIAL_DEV = "/dev/ttyACM0";

        const unsigned int QUEUE_SIZE_MAX = 20;
        const unsigned int INITAL_BUFFER_SIZE = 64;

        Settings *_settings = &Settings::Instance();

        struct termios _teensyHandle;
        int _portHandle;
        bool _deviceConfigured = false;

        queue<vector<unsigned char>> _sendQueue;

        thread _processThread;
        bool _threadActive = false;
        mutex _sendQueueMutex;
    };
}
#endif // TEENSYLIGHTCONNECTOR_H
