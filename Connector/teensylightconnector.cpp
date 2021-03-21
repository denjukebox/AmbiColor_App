#include "teensylightconnector.h"

AC::TeensyLightConnector::TeensyLightConnector()
{

}

AC::TeensyLightConnector::~TeensyLightConnector(){
    CloseDevice();
}

bool AC::TeensyLightConnector::StartBroadcast(){
    if(_threadActive || !IsConnected())
        return false;

    Logger::WriteActivity(typeid(this), "Started");

    _threadActive = true;
    _processThread = thread(Thread, _portHandle, _manager, _settings, &_threadActive);

    return true;
}

bool AC::TeensyLightConnector::StopBroadcast(){
    if(!_threadActive)
        return false;

    Logger::WriteActivity(typeid(this), "Stopped");
    _threadActive = false;
    _processThread.join();

    return true;
}

bool AC::TeensyLightConnector::IsConnected(){
    return _portHandle != 0;
}

bool AC::TeensyLightConnector::OpenDevice(){
    _portHandle = open(SERIAL_DEV, O_RDWR);

    // Check for errors
    if (_portHandle < 0) {
        Logger::WriteError(typeid(this), strerror(errno));
        return false;
    }

    memset(&_teensyHandle, 0, sizeof(struct termios));
    if(tcgetattr(_portHandle, &_teensyHandle) != 0) {
        Logger::WriteError(typeid(this), strerror(errno));
        return false;
    }

    _teensyHandle.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
    _teensyHandle.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
    _teensyHandle.c_cflag |= CS8; // 8 bits per byte (most common)
    _teensyHandle.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
    _teensyHandle.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)
    _teensyHandle.c_lflag &= ~ICANON; // Disable canonical mode
    _teensyHandle.c_lflag &= ~ECHO; // Disable echo
    _teensyHandle.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
    _teensyHandle.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    _teensyHandle.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes
    _teensyHandle.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    _teensyHandle.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
    _teensyHandle.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    _teensyHandle.c_cc[VMIN] = 0;

    // Set in/out baud rate to be 460800 115200
    cfsetispeed(&_teensyHandle, B460800);
    cfsetospeed(&_teensyHandle, B460800);

    if (tcsetattr(_portHandle, TCSANOW, &_teensyHandle) != 0) {
        Logger::WriteError(typeid(this), strerror(errno));
        return false;
    }

    Logger::WriteActivity(typeid(this), "Connected");
    return IsConnected();
}

bool AC::TeensyLightConnector::CloseDevice(){
    if(IsConnected())
        close(_portHandle);

    if(_threadActive){
        _threadActive = false;
        _processThread.join();
        Logger::WriteActivity(typeid(this), "Closed");
    }

    return !IsConnected();
}


bool AC::TeensyLightConnector::ConfigureDevice(){
    if(!IsConnected())
        return false;

   unsigned char sendbuffer[64];
   unsigned long offset = 0;
   offset = PushOnBuffer(sendbuffer, offset, 'C');
   offset = PushIntOnBuffer(sendbuffer, offset, _settings->GetWidth());
   offset = PushIntOnBuffer(sendbuffer, offset, _settings->GetHeight());

   for(int orderPos = 0; orderPos < 4; orderPos++){
       offset = PushOrderOnBuffer(sendbuffer, offset, _settings->GetOrder().at(orderPos));
   }

   offset = PushIntOnBuffer(sendbuffer, offset, _settings->GetNumberOfStrips());
   PushToTeensy(_portHandle, sendbuffer, 64);
   Logger::WriteDebug(typeid(this), "Configured");
   _deviceConfigured = true;
   return true;
}

unsigned long AC::TeensyLightConnector::PushOrderOnBuffer(unsigned char *buffer, unsigned long bufferOffset, Direction dir){
    switch(dir){
        case Direction::Top:
        bufferOffset = PushOnBuffer(buffer, bufferOffset, 'T');
        break;
        case Direction::Bottom:
        bufferOffset = PushOnBuffer(buffer, bufferOffset, 'B');
        break;
        case Direction::Left:
        bufferOffset = PushOnBuffer(buffer, bufferOffset, 'L');
        break;
        case Direction::Right:
        bufferOffset = PushOnBuffer(buffer, bufferOffset, 'R');
        break;
    }

    return bufferOffset;
}

unsigned long AC::TeensyLightConnector::PushIntOnBuffer(unsigned char *buffer, unsigned long bufferOffset, int num){
    auto fitCount = floor(num / 255);
    for(auto pos = 0; pos < 4; pos++){
        if(pos < fitCount){
            bufferOffset = PushOnBuffer(buffer, bufferOffset, char(255));
        }
        if(pos == fitCount){
            bufferOffset = PushOnBuffer(buffer, bufferOffset, char(num % 255));
        }
        if(pos > fitCount){
            bufferOffset = PushOnBuffer(buffer, bufferOffset, char(0));
        }
    }

    return bufferOffset;
}

unsigned long AC::TeensyLightConnector::PushOnBuffer(unsigned char *buffer, unsigned long bufferOffset, char value){
    *(buffer + bufferOffset) = value;
    bufferOffset++;
    return bufferOffset;
}

void AC::TeensyLightConnector::Thread(int portHandle, BufferManager* manager, Settings* settings, bool *threadActive)
{
    auto size = settings->GetBufferSize() + 1;
    while(*threadActive && portHandle != 0)
    {
        auto wrapper = manager->GetFreeResult();
        if(wrapper != nullptr){
            Statistics::Instance().NextQueued(Statistics::StatisticType::Teensy);
            unsigned char colorBuffer[size];
            *colorBuffer = 'F';
            ProccessResult(wrapper, colorBuffer, settings);
            PushToTeensy(portHandle, colorBuffer, size);
            Statistics::Instance().NextConsumed(Statistics::StatisticType::Teensy);
        }
    }
}

void AC::TeensyLightConnector::ProccessResult(ResultWrapper* wrapper, unsigned char *buffer, Settings* settings){
    unsigned long bufferOffset = 1;
    foreach (auto o, settings->GetOrder()) {
        auto colors = wrapper->GetBegin(o);
        auto colorsLength = wrapper->GetSize(o);
        ApplyCorrection(colors,
                        colorsLength,
                        settings->GetBrightnessFactor(),
                        settings->GetColorCorrectionRedFactor(),
                        settings->GetColorCorrectionGreenFactor(),
                        settings->GetColorCorrectionBlueFactor());
        bufferOffset = PushColorsOnBuffer(buffer, bufferOffset, colors, colorsLength, settings->GetColorOrder());
    }
}

void AC::TeensyLightConnector::ApplyCorrection(vector<QColor>::iterator colors, unsigned long colorsLength, double brightnessFactor, double redFactor, double greenFactor, double blueFactor){
    for(unsigned long pos = 0; pos < colorsLength; pos++){
        colors[pos].setRed(colors[pos].red() * redFactor * brightnessFactor);
        colors[pos].setGreen(colors[pos].green() * greenFactor * brightnessFactor);
        colors[pos].setBlue(colors[pos].blue() * blueFactor * brightnessFactor);
    }
}

unsigned long AC::TeensyLightConnector::PushColorsOnBuffer(unsigned char *buffer, unsigned long bufferOffset, vector<QColor>::iterator colors, unsigned long colorsLength, ColorOrder order){
    for(unsigned long pos = 0; pos < colorsLength; pos++){
        switch (order) {
            case ColorOrder::RGB:
                bufferOffset = PushOnBuffer(buffer, bufferOffset, char(colors[pos].red()));
                bufferOffset = PushOnBuffer(buffer, bufferOffset, char(colors[pos].green()));
                bufferOffset = PushOnBuffer(buffer, bufferOffset, char(colors[pos].blue()));
                break;
            case ColorOrder::RBG:
                bufferOffset = PushOnBuffer(buffer, bufferOffset, char(colors[pos].red()));
                bufferOffset = PushOnBuffer(buffer, bufferOffset, char(colors[pos].blue()));
                bufferOffset = PushOnBuffer(buffer, bufferOffset, char(colors[pos].green()));
                break;
            case ColorOrder::GRB:
                bufferOffset = PushOnBuffer(buffer, bufferOffset, char(colors[pos].green()));
                bufferOffset = PushOnBuffer(buffer, bufferOffset, char(colors[pos].red()));
                bufferOffset = PushOnBuffer(buffer, bufferOffset, char(colors[pos].blue()));
                break;
            case ColorOrder::GBR:
                bufferOffset = PushOnBuffer(buffer, bufferOffset, char(colors[pos].green()));
                bufferOffset = PushOnBuffer(buffer, bufferOffset, char(colors[pos].blue()));
                bufferOffset = PushOnBuffer(buffer, bufferOffset, char(colors[pos].red()));
                break;
        }
    }

    return bufferOffset;
}

bool AC::TeensyLightConnector::PushToTeensy(int _portHandle, unsigned char* buffer, long size)
{
    try
    {
        if(_portHandle == 0 || size != write(_portHandle, buffer, size))
            throw exception();
        return true;
    }
    catch (const exception& ex)
    {
        Logger::WriteError(typeid(AC::TeensyLightConnector), ex.what());
        return false;
    }
}

//void AC::TeensyLightConnector::SendBuffer(int _portHandle, queue<vector<unsigned char>> *sendQueue, mutex* sendMutex, bool *threadActive)
//{
//    while(*threadActive && _portHandle != 0)
//    {
//        if(!sendQueue->empty())
//        {
//            sendMutex->lock();
//            Statistics::Instance().NextConsumed(Statistics::StatisticType::Teensy);
//            PushToTeensy(_portHandle, sendQueue->front().begin(), sendQueue->front().end());
//            sendQueue->pop();
//            sendMutex->unlock();
//        }
//    }
//}

//bool AC::TeensyLightConnector::PushOnSendQueue(char command){
//    auto buffer = vector<unsigned char>(GetBufferSize());
//    buffer[0] = command;
//    PushOnSendQueue(buffer);
//    return true;
//}

//bool AC::TeensyLightConnector::PushOnSendQueue(char command, vector<unsigned char>::iterator start, vector<unsigned char>::iterator end)
//{
//    auto buffer = vector<unsigned char>(GetBufferSize());
//    copy(start, end, buffer.begin() + sizeof (unsigned char));
//    buffer[0] = command;
//    return PushOnSendQueue(buffer);
//}

//bool AC::TeensyLightConnector::PushOnSendQueue(const vector<unsigned char> buffer)
//{
//    try
//    {
//         _sendQueueMutex.lock();
//        while(QUEUE_SIZE_MAX <= _sendQueue.size())
//        {
//            Logger::WriteLine(typeid(this), "SendBuffer Overflow");
//            sleep(1); //Sleep this thread a second so controller has time to clear last buffer
//        }

//        _sendQueue.push(buffer);
//        _sendQueueMutex.unlock();
//        return true;
//    }
//    catch (const exception& ex) {
//        _sendQueueMutex.unlock();
//        Logger::WriteLine(typeid(this), ex.what());
//        return false;
//    }
//}

//int AC::TeensyLightConnector::GetBufferSize(){
//    return _settings->GetBufferSize();//_deviceConfigured ? _settings->GetBufferSize() : INITAL_BUFFER_SIZE;
//}
