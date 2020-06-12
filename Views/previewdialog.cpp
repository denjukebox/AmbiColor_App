#include "previewdialog.h"
#include "ui_previewdialog.h"

PreviewDialog::PreviewDialog(QWidget *parent) :
    QDialog(parent),
    _ui(new Ui::PreviewDialog)
{
    _ui->setupUi(this);
}

void PreviewDialog::showEvent(QShowEvent *event) {
    StartPreview();
}

void PreviewDialog::hideEvent(QHideEvent *event) {
    StopPreview();
}

void PreviewDialog::StartPreview(){
    if(_isRunning)
        return;

    _previewThreader.StartThread();
    _grid->StartPreview();
    _isRunning = true;
}

void PreviewDialog::StopPreview(){
    if(!_isRunning)
        return;

    _grid->StopPreview();
    _previewThreader.StopThread();
    _isRunning = false;
}


PreviewDialog::~PreviewDialog()
{
    StopPreview();
    delete _ui;
}
