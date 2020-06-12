#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <chrono>
#include <vector>
#include "Views/configuration.h"
#include "Capture/enums.h"

using namespace std;

namespace AC {
    class Settings
    {
    public:
        Settings();

        static Settings &Instance() {static Settings buffer; return buffer;}

        ////Sets
        //Capture
        void SetWidth(int width);
        void SetHeight(int height);
        void SetDepth(int depth);
        void SetContentRatio(double ratio);
        void SetCaptureRate(chrono::milliseconds rate);
        void SetColorOrder(ColorOrder order);
        void SetColorCorrection(LEDColorCorrection correction);

        //Controller
        void SetBrightness(int brightness);
        void SetNumberOfStrips(int numberOfStrips);
        void SetBufferSize(int bufferSize);
        void SetOrder(vector<Direction> order);
        void SetBroadcastRate(chrono::milliseconds rate);

        //Preview
        void SetPreviewRate(chrono::milliseconds rate);

        ////Gets
        //Capture
        int GetWidth();
        int GetHeight();
        int GetDepth();
        double GetContentRatio();
        chrono::milliseconds GetCaptureRate();

        //Controller
        int GetBrightness();
        int GetNumberOfStrips();
        int GetBufferSize();
        int GetAmoutPixels();
        vector<Direction> GetOrder();
        chrono::milliseconds GetBroadcastRate();
        ColorOrder GetColorOrder();

        LEDColorCorrection GetColorCorrection();
        float GetBrightnessFactor();
        float GetColorCorrectionRedFactor();
        float GetColorCorrectionGreenFactor();
        float GetColorCorrectionBlueFactor();

        //Preview
        chrono::milliseconds GetPreviewRate();

    private:
        //Capture
        int _width = STARTUP_WIDTH;
        int _height = STARTUP_HEIGHT;
        int _depth = STARTUP_DEPTH;
        double _ratio = STARTUP_RATIO;
        ColorOrder _colorOrder = STARTUP_COLOR_ORDER;
        LEDColorCorrection _correction = STARTUP_COLOR_CORRECTION;
        chrono::milliseconds _captureRate = chrono::milliseconds(1000 / CAPTURE_FPS);

        //Controller
        int _brightness = CONTROLLER_BRIGHTNESS;
        int _numberOfStrips = CONTROLLER_STRIP_COUNT;
        vector<Direction> _order = CONTROLLER_ORDER;
        chrono::milliseconds _broadcastRate = chrono::milliseconds(1000 / CONTROLLER_FPS);

        //Preview
        chrono::milliseconds _previewRate = chrono::milliseconds(1000 / PREVIEW_FPS);

    };
}

#endif // SETTINGS_H
