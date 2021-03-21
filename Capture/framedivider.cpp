#include "framedivider.h"

AC::FrameDivider::FrameDivider()
{
}

vector<QColor> AC::FrameDivider::CalculateHorizontalRow(vector<Screen_Capture::ImageBGRA>::iterator imgRef, unsigned int blocksWidth, unsigned int imageWidth, unsigned int offsetHeight, unsigned int depth, unsigned int pixelPerblock)
{
    vector<QColor> colors;

    for(unsigned int blockPos = 0; blockPos < blocksWidth; blockPos++)
    {
        colors.push_back(CalculateHorizontalPixel(
                        imgRef,
                        offsetHeight,
                        blockPos,
                        imageWidth,
                        depth,
                        pixelPerblock));
    }
    return colors;
}

vector<QColor> AC::FrameDivider::CalculateVerticalRow(vector<Screen_Capture::ImageBGRA>::iterator imgRef, unsigned int blocksHeight, unsigned int imageWidth, unsigned int imageHeight, unsigned int offsetWidth, unsigned int depth, unsigned int pixelPerblock)
{
    vector<QColor> colors;

    for(unsigned int blockPos = 0; blockPos < blocksHeight; blockPos++)
    {
        colors.push_back(CalculateVerticalPixel(
                        imgRef,
                        offsetWidth,
                        blockPos,
                        imageWidth,
                        imageHeight,
                        depth,
                        pixelPerblock));
    }
    return colors;
}

QColor AC::FrameDivider::CalculateHorizontalPixel(vector<Screen_Capture::ImageBGRA>::iterator imgRef, unsigned int rowOffset, unsigned int blockPos, unsigned int imageWidth, unsigned int depth, unsigned int pixelPerblockWidth)
{
    AverageColor color;
    vector<Screen_Capture::ImageBGRA> relevantColors;

    auto blockOffset = blockPos * pixelPerblockWidth;
    if(imageWidth < blockOffset + pixelPerblockWidth)
        pixelPerblockWidth = imageWidth - blockOffset;
    for (unsigned int depthPos = 0; depthPos < depth; depthPos++) {
        auto blockStart = imgRef + //Start of image
                            ((rowOffset + depthPos) * imageWidth) + //Row offset
                            (pixelPerblockWidth * blockPos);
        relevantColors = vector<Screen_Capture::ImageBGRA>(blockStart, blockStart + pixelPerblockWidth);
        color.AddToAverage(relevantColors.begin(), relevantColors.size());
    }

    return color.GetQColor();
}

QColor AC::FrameDivider::CalculateVerticalPixel(vector<Screen_Capture::ImageBGRA>::iterator imgRef, unsigned int columnOffset, unsigned int blockPos, unsigned int imageWidth, unsigned int imageHeight, unsigned int depth, unsigned int pixelPerblockHeight)
{
    AverageColor color;
    vector<Screen_Capture::ImageBGRA> relevantColors;

    auto blockOffset = blockPos * pixelPerblockHeight;
    if(imageHeight < blockOffset + pixelPerblockHeight)
        pixelPerblockHeight = imageHeight - blockOffset;
    for (unsigned int rowPos = 0; rowPos < pixelPerblockHeight; rowPos++) {
        auto blockStart = imgRef + //Start of image
                            (blockOffset + rowPos) * imageWidth + //Row offset
                            columnOffset;
        relevantColors = vector<Screen_Capture::ImageBGRA>(blockStart, blockStart + depth);
        color.AddToAverage(relevantColors.begin(), relevantColors.size());
    }

    return color.GetQColor();
}

unsigned int AC::FrameDivider::CalculateVerticalOffset(unsigned int imageWidth, unsigned int imageHeight, double contentRatio){
    if(contentRatio == 1.0 || imageWidth < imageHeight)
        return 1;

    auto contentHeight = imageWidth * contentRatio;
    return (imageHeight - contentHeight)/2;
}

unsigned int AC::FrameDivider::CalculateHorizontalOffset(unsigned int imageWidth, unsigned int imageHeight, double contentRatio){
    if(contentRatio == 1.0 || imageWidth > imageHeight)
        return 1;

    auto contentWidth = imageHeight * contentRatio;
    return (imageWidth - contentWidth)/2;
}

unsigned int AC::FrameDivider::PixelsPerBlockHeight(unsigned int imageHeight, unsigned int blocksHeight){
    return floor(imageHeight / blocksHeight);
}

unsigned int AC::FrameDivider::PixelsPerBlockWith(unsigned int imageWidth, unsigned int blocksWidth){
    return floor(imageWidth / blocksWidth);
}
