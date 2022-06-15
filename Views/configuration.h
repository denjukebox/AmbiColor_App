#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "Capture/enums.h"
#include <vector>

namespace AC {
    const static int STARTUP_WIDTH = 124;
    const static int STARTUP_HEIGHT = 66;
    const static int STARTUP_DEPTH = 50;
    const static int STARTUP_TIME_SMOOTHING = 10;
    const static bool STARTUP_IS_DIVIDED_ASYNC = true;
    const static double STARTUP_RATIO = 1.0;
    const static ColorOrder STARTUP_COLOR_ORDER = ColorOrder::GRB;
    const static LEDColorCorrection STARTUP_COLOR_CORRECTION = LEDColorCorrection::TypicalSMD5050;

    const static int CAPTURE_FPS = 60;

    const static int CONTROLLER_BRIGHTNESS = 130;
    const static int CONTROLLER_STRIP_COUNT = 2;
    const static std::vector<Direction> CONTROLLER_ORDER =
            std::vector<Direction>{ Direction::Bottom, Direction::Left, Direction::Right, Direction::Top };

    const static int PREVIEW_FPS = 60;
}

#endif // CONFIGURATION_H
