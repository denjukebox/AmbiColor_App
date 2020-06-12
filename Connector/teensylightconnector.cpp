#include "teensylightconnector.h"

AC::TeensyLightConnector::TeensyLightConnector()
{

}

AC::TeensyLightConnector::~TeensyLightConnector(){
    CloseDevice();
}

bool AC::TeensyLightConnector::OpenDevice(){
    _portHandle = open(SERIAL_DEV, O_RDWR);

    // Check for errors
    if (_portHandle < 0) {
        printf("Error %i from open: %s\n", errno, strerror(errno));
        return false;
    }

    memset(&_teensyHandle, 0, sizeof(struct termios));
    if(tcgetattr(_portHandle, &_teensyHandle) != 0) {
        printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
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
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        return false;
    }

    if(!_threadActive && IsConnected()){
        _threadActive = true;
        _processThread = thread(SendBuffer, _portHandle, &_sendQueue, &_sendQueueMutex, &_threadActive);
        cout << "CONNECTOR:Connected" << endl;
    }

    return IsConnected();
}

bool AC::TeensyLightConnector::CloseDevice(){
    if(IsConnected())
        close(_portHandle);

    if(_threadActive){
        _threadActive = false;
        _processThread.join();
        cout << "CONNECTOR:Closed" << endl;
    }

    return !IsConnected();
}

bool AC::TeensyLightConnector::IsConnected(){
    return _portHandle != 0;
}

int AC::TeensyLightConnector::GetBufferSize(){
    return _deviceConfigured ? _settings->GetBufferSize() : INITAL_BUFFER_SIZE;
}

void AC::TeensyLightConnector::SendBuffer(int _portHandle, queue<vector<unsigned char>> *sendQueue, mutex* sendMutex, bool *threadActive)
{
    while(*threadActive && _portHandle != 0)
    {
        if(!sendQueue->empty())
        {
            sendMutex->lock();
            Statistics::Instance().NextConsumed(Statistics::StatisticType::Teensy);
            PushToTeensy(_portHandle, sendQueue->front().begin(), sendQueue->front().end());
            sendQueue->pop();            
            sendMutex->unlock();
        }
    }
}

bool AC::TeensyLightConnector::PushToTeensy(int _portHandle, vector<unsigned char>::iterator start, vector<unsigned char>::iterator end)
{
    try
    {    auto size = distance(start, end);
        unsigned char buffer[size];
        copy(start, end, buffer);
        if(_portHandle == 0 || size != write(_portHandle, buffer, size))
            throw exception();
    }
    catch (const exception& ex)
    {
        cout << "CONNECTOR:"<< ex.what() << endl;
        return false;
    }
    return true;
}

bool AC::TeensyLightConnector::PushOnSendQueue(char command){
    auto buffer = vector<unsigned char>(GetBufferSize());
    buffer[0] = command;
    PushOnSendQueue(buffer);
    return true;
}

bool AC::TeensyLightConnector::PushOnSendQueue(char command, vector<unsigned char>::iterator start, vector<unsigned char>::iterator end)
{
    auto buffer = vector<unsigned char>(GetBufferSize());
    copy(start, end, buffer.begin() + sizeof (unsigned char));
    buffer[0] = command;
    return PushOnSendQueue(buffer);
}

bool AC::TeensyLightConnector::PushOnSendQueue(const vector<unsigned char> buffer)
{
    try
    {
         _sendQueueMutex.lock();
        while(QUEUE_SIZE_MAX <= _sendQueue.size())
        {
            cout << "CONNECTOR:SendBuffer Overflow" << endl;
            sleep(1); //Sleep this thread a second so controller has time to clear last buffer
        }

        _sendQueue.push(buffer);
        _sendQueueMutex.unlock();
        return true;
    }
    catch (const exception& ex) {

        _sendQueueMutex.unlock();
        cout << "CONNECTOR:"<< ex.what() << endl;
        return false;
    }
}

bool AC::TeensyLightConnector::ConfigureDevice(){
    if(!IsConnected())
        return false;

   auto sendbuffer = vector<unsigned char>();
   PushIntOnBuffer(&sendbuffer, _settings->GetWidth());
   PushIntOnBuffer(&sendbuffer, _settings->GetHeight());

   for(int orderPos = 0; orderPos < 4; orderPos++){
       switch(_settings->GetOrder().at(orderPos)){
           case Direction::Top:
           sendbuffer.push_back('T');
           break;
           case Direction::Bottom:
           sendbuffer.push_back('B');
           break;
           case Direction::Left:
           sendbuffer.push_back('L');
           break;
           case Direction::Right:
           sendbuffer.push_back('R');
           break;
       }
   }

   sendbuffer.push_back(_settings->GetNumberOfStrips());
   PushOnSendQueue('C', sendbuffer.begin(), sendbuffer.end());
   cout << "CONNECTOR:Configured" << endl;
   _deviceConfigured = true;
   return true;
}

void AC::TeensyLightConnector::PushIntOnBuffer(vector<unsigned char> *buffer, int num){
    auto fitCount = floor(num / 255);
    for(auto pos = 0; pos < 4; pos++){
        if(pos < fitCount){
            buffer->push_back(255);
        }
        if(pos == fitCount){
            buffer->push_back(num % 255);
        }
        if(pos > fitCount){
            buffer->push_back(0);
        }
    }
}
