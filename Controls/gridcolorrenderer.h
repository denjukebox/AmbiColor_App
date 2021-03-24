#ifndef GRIDCOLORRENDERER_H
#define GRIDCOLORRENDERER_H

#include <QObject>
#include <QGridLayout>
#include "simplerectangle.h"
#include "Capture/averagecolor.h"
#include "Capture/resultwrapper.h"
#include "Capture/buffers.h"
#include "Util/statistics.h"
#include "Util/settings.h"

using namespace std;

namespace AC {
    class GridColorRenderer : public QGridLayout
    {
        Q_OBJECT

    public:
        explicit GridColorRenderer(ResultManager *resultManager, QWidget *parent);
        ~GridColorRenderer();

        void Start();
        void Stop();

    signals:
        void WorkDone();

    private slots:
        void UpdateRectangles();

    protected:
        void paintEvent(QPaintEvent *event);

    private:
        ResultManager *_resultManager;
        Settings *_settings = &Settings::Instance();

        bool _threadActive;
        std::thread _processThread;

        static void SetColors(GridColorRenderer *self, ResultManager *resultManager, Settings *settings, bool *threadActive);
        void static PushColorsToGrid(vector<QColor>::iterator colors, vector<SimpleRectangle*> rectangles);

        vector<SimpleRectangle*> FillHorizontal(vector<SimpleRectangle*> rectangles, int verticalStart, int verticalOffset);
        vector<SimpleRectangle*> FillVertical(vector<SimpleRectangle*> rectangles, int horizontalStart, int horizontalOffset);
        vector<SimpleRectangle*> FillLine(vector<SimpleRectangle*> rectangles, bool isHorizontal, int limit, int start, int offset);

        bool _isGridFilled = false;
        void FillGrid();
        void ClearGrid();

        vector<SimpleRectangle*> _top;
        vector<SimpleRectangle*> _bottom;
        vector<SimpleRectangle*> _left;
        vector<SimpleRectangle*> _right;
    };
}

#endif // GRIDCOLORRENDERER_H
