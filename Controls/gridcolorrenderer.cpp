#include "gridcolorrenderer.h"

AC::GridColorRenderer::GridColorRenderer(QWidget *parent) : QGridLayout(parent)
{
    this->setSpacing(0);
    this->setMargin(0);
    connect(this, &AC::GridColorRenderer::WorkDone, this, &AC::GridColorRenderer::UpdateRectangles);
    FillGrid();
}

void AC::GridColorRenderer::Start(){
    if(_threadActive || !_isGridFilled)
        return;

    _threadActive = true;
    _processThread = std::thread(SetColors, this, _manager, _settings, &_threadActive);
}

void AC::GridColorRenderer::Stop(){
    if(!_threadActive)
        return;

    _threadActive = false;
    _processThread.join();
}

void AC::GridColorRenderer::SetColors(GridColorRenderer *self, BufferManager *manager, Settings *settings, bool *threadActive)
{
    ResultWrapper* wrapper;
    while (*threadActive)
    {
        wrapper = manager->GetFreeResult();
        if(wrapper != nullptr){
            Statistics::Instance().NextConsumed(Statistics::StatisticType::Result);
            PushColorsToGrid(wrapper->GetTopBegin(), self->_top);
            PushColorsToGrid(wrapper->GetBottomBegin(), self->_bottom);
            PushColorsToGrid(wrapper->GetLeftBegin(), self->_left);
            PushColorsToGrid(wrapper->GetRightBegin(), self->_right);
            manager->CleanResult(wrapper);
            emit self->WorkDone();
        }
        this_thread::sleep_for(settings->GetPreviewRate());
    }
}

void AC::GridColorRenderer::UpdateRectangles(){
    this->parentWidget()->repaint();
}

void AC::GridColorRenderer::PushColorsToGrid(vector<QColor>::iterator colors, vector<SimpleRectangle*> rectangles)
{
    for ( unsigned long m = 0 ; m < rectangles.size() ; m++, colors++ ) {
        rectangles.at(m)->setColors(colors->red(), colors->green(), colors->blue());
    }
}

vector<SimpleRectangle*> AC::GridColorRenderer::FillHorizontal(vector<SimpleRectangle*> rectangles, int horizontalStart, int verticalOffset)
{
    return FillLine(rectangles, true, _settings->GetWidth(), horizontalStart, verticalOffset);
}

vector<SimpleRectangle*> AC::GridColorRenderer::FillVertical(vector<SimpleRectangle*> rectangles, int verticalStart, int horizontalOffset)
{
    return FillLine(rectangles, false, _settings->GetHeight(), verticalStart, horizontalOffset);
}

vector<SimpleRectangle*> AC::GridColorRenderer::FillLine(vector<SimpleRectangle*> rectangles, bool isHorizontal, int limit, int start, int offset)
{
    for ( auto m = start ; m < (limit+start) ; m++ ) {
        rectangles.push_back(new SimpleRectangle());
        if(isHorizontal)
            this->addWidget(rectangles.back(), offset, m);
        else
            this->addWidget(rectangles.back(), m, offset);
    }

    return rectangles;
}

void AC::GridColorRenderer::FillGrid()
{
    _top = FillHorizontal(_top, 0, 0);
    _bottom = FillHorizontal(_bottom, 0,(_settings->GetHeight() + 2));
    _left = FillVertical(_left, 2,0);
    _right = FillVertical(_right, 2, (_settings->GetWidth() - 1));
    _isGridFilled = true;
}

void AC::GridColorRenderer::ClearGrid()
{
    while(this->count() != 0)
        delete this->itemAt(0)->widget();
    _top.clear();
    _bottom.clear();
    _left.clear();
    _right.clear();
    _isGridFilled = false;
}

AC::GridColorRenderer::~GridColorRenderer(){

}

