#ifndef PREVIEWDIALOG_H
#define PREVIEWDIALOG_H

#include <QDialog>
#include "Controls/gridcolorrenderer.h"
#include "Capture/divisionthreader.h"
#include "Util/settings.h"

using namespace AC;

namespace Ui {
    class PreviewDialog;
}

class PreviewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreviewDialog(FrameManager *frameManager, ResultManager *resultManager, QWidget *parent = nullptr);
    ~PreviewDialog();

protected:
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);

private:
    void Start();
    void Stop();

    Settings *_settings = &Settings::Instance();
    Ui::PreviewDialog *_ui;
    GridColorRenderer _grid;// = new GridColorRenderer(this, _resultManager);
    DivisionThreader _divisionThreader;

    bool _isRunning = false;
};


#endif // PREVIEWDIALOG_H
