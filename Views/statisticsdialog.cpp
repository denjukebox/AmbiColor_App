#include "statisticsdialog.h"
#include "ui_statisticsdialog.h"

StatisticsDialog::StatisticsDialog(QWidget *parent) :
    QDialog(parent),
    _ui(new Ui::StatisticsDialog)
{
    _ui->setupUi(this);
}

void StatisticsDialog::showEvent(QShowEvent *event) {
    StartStatisticsUpdate();
}

void StatisticsDialog::hideEvent(QHideEvent *event) {
    StopStatisticsUpdate();
}

void StatisticsDialog::StartStatisticsUpdate(){
    if(_threadActive)
        return;

    _threadActive = true;
    _processThread = std::thread(SetStatisticData, this, &_threadActive);
}

void StatisticsDialog::StopStatisticsUpdate(){
    if(!_threadActive)
        return;

    _threadActive = false;
    _processThread.join();
}

void StatisticsDialog::SetStatisticData(StatisticsDialog *dialog, bool *threadActive){
    while (*threadActive) {
        auto frameQueueTime = Statistics::Instance().GetQueueTime(Statistics::StatisticType::Frame);
        auto frameConsumeTime = Statistics::Instance().GetConsumeTime(Statistics::StatisticType::Frame);
        auto teensyQueueTime = Statistics::Instance().GetQueueTime(Statistics::StatisticType::Teensy);
        auto resultsQueueTime = Statistics::Instance().GetQueueTime(Statistics::StatisticType::Result);
        auto resultsConsumeTime = Statistics::Instance().GetConsumeTime(Statistics::StatisticType::Result);
        if(frameQueueTime.count() != 0)
        dialog->_ui->frameQueuedCount->setText(
                    QString::number(frameQueueTime.count()));
        if(frameConsumeTime.count() != 0)
        dialog->_ui->frameConsumedCount->setText(
                    QString::number(frameConsumeTime.count()));
        if(teensyQueueTime.count() != 0)
        dialog->_ui->teensyQueuedCount->setText(
                    QString::number(teensyQueueTime.count()));
        if(resultsQueueTime.count() != 0)
        dialog->_ui->resultQueuedCount->setText(
                    QString::number(resultsQueueTime.count()));
        if(resultsConsumeTime.count() != 0)
        dialog->_ui->resultConsumedCount->setText(
                    QString::number(resultsConsumeTime.count()));

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

StatisticsDialog::~StatisticsDialog()
{
    StopStatisticsUpdate();
    delete _ui;
}
