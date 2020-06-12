#ifndef ENUMS_H
#define ENUMS_H

enum class ColorOrder {RGB, RBG, GRB, GBR};
enum class Direction { Top, Bottom, Left, Right };
enum class LEDColorCorrection {
    TypicalSMD5050=0xFFB0F0 /* 255, 176, 240 */,
    TypicalLEDStrip=0xFFB0F0 /* 255, 176, 240 */,
    Typical8mmPixel=0xFFE08C /* 255, 224, 140 */,
    TypicalPixelString=0xFFE08C /* 255, 224, 140 */
};

#endif // ENUMS
