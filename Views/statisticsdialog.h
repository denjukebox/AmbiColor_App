#ifndef STATISTICSDIALOG_H
#define STATISTICSDIALOG_H

#include "Util/statistics.h"
#include <thread>
#include <QDialog>

using namespace AC;

namespace Ui {
    class StatisticsDialog;
}

class StatisticsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StatisticsDialog(QWidget *parent = nullptr);
    ~StatisticsDialog();

protected:
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);

private:

    void StartStatisticsUpdate();
    void StopStatisticsUpdate();

    static void SetStatisticData(StatisticsDialog *dialog, bool *threadActive);

    bool _threadActive = false;
    std::thread _processThread;

    Ui::StatisticsDialog *_ui;
};

#endif // STATISTICSDIALOG_H
