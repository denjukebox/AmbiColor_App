#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Connector/teensylightconnector.h"

#include "Capture/teensythreader.h"
#include "Capture/framegrabber.h"

#include "Util/settings.h"

#include "Views/settingsdialog.h"
#include "Views/statisticsdialog.h"
#include "Views/previewdialog.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <climits>
#include <iostream>
#include <locale>
#include <string>
#include <thread>
#include <vector>
#include <QtCore>
#include <QSystemTrayIcon>
#include <QMessageBox>

using namespace AC;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void SetVisible(bool visible);
    void ShowTrayMessage(QString title, QString message, QSystemTrayIcon::MessageIcon icon);

public slots:
    void ToggleBroadCast();

    void ShowPreview();
    void ShowSettings();
    void ShowStatistics();

    void ExitApp();

protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *_ui;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    Settings* _settings = &Settings::Instance();

    PreviewDialog *_previewDialog = new PreviewDialog(this);
    SettingsDialog *_settingsDialog = new SettingsDialog(this);
    StatisticsDialog *_statisticsDialog = new StatisticsDialog(this);

    FrameGrabber _framegrabber = FrameGrabber();
    DivisionThreader _divisionThreader = DivisionThreader();
    TeensyLightConnector *_connector = new TeensyLightConnector();

    QAction *restoreAction;

    void SetupUi();

private slots:
    void exit();
};

#endif // MAINWINDOW_H
