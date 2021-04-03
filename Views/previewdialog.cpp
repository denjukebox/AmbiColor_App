#include "previewdialog.h"
#include "ui_previewdialog.h"

PreviewDialog::PreviewDialog(ResultManager *resultManager, QWidget *parent) :
    QDialog(parent),
    _ui(new Ui::PreviewDialog),
    _grid(resultManager, this)
{
    _ui->setupUi(this);
}

void PreviewDialog::showEvent(QShowEvent *event) {
    event->setAccepted(true);
    Start();
}

void PreviewDialog::hideEvent(QHideEvent *event) {
    event->setAccepted(true);
    Stop();
}

void PreviewDialog::Start(){
    if(_isRunning)
        return;
    _grid.Start();
    _isRunning = true;
}

void PreviewDialog::Stop(){
    if(!_isRunning)
        return;

    _grid.Stop();
    _isRunning = false;
}


PreviewDialog::~PreviewDialog()
{
    Stop();
    delete _ui;
}
