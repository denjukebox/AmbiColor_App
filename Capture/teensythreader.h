#ifndef TEENSYDIVIDER_H
#define TEENSYDIVIDER_H

//#include "averagecolor.h"
//#include "ScreenCapture.h"
//#include "framewrapper.h"
//#include "resultwrapper.h"
//#include <Capture/buffermanager.h>
//#include <Capture/framedivider.h>
//#include "Connector/teensylightconnector.h"
//#include <Util/settings.h>
//#include <Util/statistics.h>
//#include <Util/hidapi.h>
//#include "enums.h"
//#include <thread>
//#include <future>

//using namespace std;
//using namespace SL;

//namespace AC {
//    class TeensyThreader
//    {

//    public:
//        TeensyThreader(TeensyLightConnector* connector);

//        void StartBroadcast();
//        void StopBroadcast();

//        bool IsRunning();

//    private:
//        static void DivideColors(FrameDivider *divider, TeensyLightConnector *connector, BufferManager *manager, Settings *settings, bool *threadActive);

//        static vector<QColor> CalculateTop(FrameDivider *divider, vector<Screen_Capture::ImageBGRA>::iterator imgRef, Settings *settings, int blocksWidth, int imageWidth, int offsetHeight, int depth, int pixelsPerBlock, bool inverted);
//        static vector<QColor> CalculateBottom(FrameDivider *divider, vector<Screen_Capture::ImageBGRA>::iterator imgRef, Settings *settings, int blocksHeight, int imageWidth, int imageHeight, int offsetWidth, int depth, int pixelsPerBlock, bool inverted);
//        static vector<QColor> CalculateLeft(FrameDivider *divider, vector<Screen_Capture::ImageBGRA>::iterator imgRef, Settings *settings, int blocksWidth, int imageWidth, int offsetHeight, int depth, int pixelsPerBlock, bool inverted);
//        static vector<QColor> CalculateRight(FrameDivider *divider, vector<Screen_Capture::ImageBGRA>::iterator imgRef, Settings *settings, int blocksHeight, int imageWidth, int imageHeight, int offsetWidth, int depth, int pixelsPerBlock, bool inverted);

//        static vector<QColor> CalculateHorizontalRow(FrameDivider *divider, vector<Screen_Capture::ImageBGRA>::iterator imgRef, Settings *settings, int blocksWidth, int imageWidth, int offsetHeight, int depth, int pixelsPerBlock, bool inverted);
//        static vector<QColor> CalculateVerticalRow(FrameDivider *divider, vector<Screen_Capture::ImageBGRA>::iterator imgRef, Settings *settings, int blocksHeight, int imageWidth, int imageHeight, int offsetWidth, int depth, int pixelsPerBlock, bool inverted);

//        FrameDivider _divider = FrameDivider();
//        Settings *_settings = &Settings::Instance();
//        BufferManager *_manager = &BufferManager::Instance();
//        TeensyLightConnector *_connector;

//        thread _processThread;
//        bool _threadActive = false;
//    };

//}

#endif // TEENSYDIVIDER_H
