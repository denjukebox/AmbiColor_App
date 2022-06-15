#include "teensylightconnector.h"

AC::TeensyLightConnector::TeensyLightConnector(ResultManager *resultManager)
{
    _resultManager = resultManager;
}

AC::TeensyLightConnector::~TeensyLightConnector(){
    CloseDevice();
}

bool AC::TeensyLightConnector::StartBroadcast(){
    if(_threadActive || !IsConnected())
        return false;

    Logger::WriteActivity(typeid(this), "Started");

    _threadActive = true;
    _processThread = thread(Thread, _portHandle, _resultManager, _settings, &_threadActive);

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

   unsigned char sendbuffer[CONFIG_FRAME_SERIAL_RX_SIZE];
   unsigned long offset = PushCommandOnBuffer(sendbuffer, COMMAND_CONFIG);
   offset = PushIntOnBuffer(sendbuffer, offset, _settings->GetWidth());
   offset = PushIntOnBuffer(sendbuffer, offset, _settings->GetHeight());
   offset = PushIntOnBuffer(sendbuffer, offset, (int)_settings->GetColorOrder());
   PushIntOnBuffer(sendbuffer, offset, _settings->GetNumberOfStrips());

   PushToTeensy(_portHandle, sendbuffer, CONFIG_FRAME_SERIAL_RX_SIZE);
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


unsigned long AC::TeensyLightConnector::PushCommandOnBuffer(unsigned char *buffer, char command){
    int offset = 0;
    offset = PushOnBuffer(buffer, offset, CONTROL);
    offset = PushOnBuffer(buffer, offset, command);
    return PushOnBuffer(buffer, offset, CONTROL);
}

unsigned long AC::TeensyLightConnector::PushOnBuffer(unsigned char *buffer, unsigned long bufferOffset, char value){
    *(buffer + bufferOffset) = value;
    bufferOffset++;
    return bufferOffset;
}

void AC::TeensyLightConnector::Thread(int portHandle, ResultManager *resultManager, Settings* settings, bool *threadActive)
{
    auto size = settings->GetBufferSize() + COMMAND_BUFFER_OFFSET;
    unsigned char colorBuffer[size];
    PushCommandOnBuffer(colorBuffer, COMMAND_FRAME);
    while(*threadActive && portHandle != 0)
    {
        auto wrapper = resultManager->GetFree();
        if(wrapper != nullptr){
            Statistics::Instance().NextQueued(Statistics::StatisticType::Teensy);
            int offset = COMMAND_BUFFER_OFFSET;
            ProccessResult(wrapper, colorBuffer, offset, settings);
            PushToTeensy(portHandle, colorBuffer, size);
            Statistics::Instance().NextConsumed(Statistics::StatisticType::Teensy);
            resultManager->Clean(wrapper);
        }
    }
}

void AC::TeensyLightConnector::ProccessResult(ResultWrapper* wrapper, unsigned char *buffer, int offset, Settings* settings){
    foreach (auto o, settings->GetOrder()) {
        auto colors = wrapper->GetBegin(o);
        auto colorsLength = wrapper->GetSize(o);
        ApplyCorrection(colors,
                        colorsLength,
                        settings->GetBrightnessFactor(),
                        settings->GetColorCorrectionRedFactor(),
                        settings->GetColorCorrectionGreenFactor(),
                        settings->GetColorCorrectionBlueFactor());
        offset = PushColorsOnBuffer(buffer, offset, colors, colorsLength, settings->GetColorOrder());
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
        bufferOffset = PushOnBuffer(buffer, bufferOffset, char(colors[colorsLength - pos].red()));
        bufferOffset = PushOnBuffer(buffer, bufferOffset, char(colors[colorsLength - pos].green()));
        bufferOffset = PushOnBuffer(buffer, bufferOffset, char(colors[colorsLength - pos].blue()));
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
