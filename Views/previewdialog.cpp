#include "previewdialog.h"
#include "ui_previewdialog.h"

PreviewDialog::PreviewDialog(FrameManager *frameManager, ResultManager *resultManager, QWidget *parent) :
    QDialog(parent),
    _ui(new Ui::PreviewDialog),
    _grid(resultManager, this),
    _divisionThreader(frameManager, resultManager)
{
    _ui->setupUi(this);
}

void PreviewDialog::showEvent(QShowEvent *event) {
    Start();
}

void PreviewDialog::hideEvent(QHideEvent *event) {
    Stop();
}

void PreviewDialog::Start(){
    if(_isRunning)
        return;

    _divisionThreader.Start();
    _grid.Start();
    _isRunning = true;
}

void PreviewDialog::Stop(){
    if(!_isRunning)
        return;

    _grid.Stop();
    _divisionThreader.Stop();
    _isRunning = false;
}


PreviewDialog::~PreviewDialog()
{
    Stop();
    delete _ui;
}
