#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::MainWindow)
{
    SetupUi();
    _framegrabber.StartCapture();
}

void MainWindow::SetupUi()
{
    _ui->setupUi(this);
    setWindowFlag(Qt::CustomizeWindowHint, true);
    setWindowFlag(Qt::WindowCloseButtonHint, true);
    setWindowFlag(Qt::WindowMaximizeButtonHint, false);
    setWindowFlag(Qt::WindowMinimizeButtonHint, false);

    //Action connections
    connect(_ui->actionStartStopBroadcast, SIGNAL(triggered()), this, SLOT(ToggleBroadCast()));
    connect(_ui->actionPreview, SIGNAL(triggered()), this, SLOT(ShowPreview()));
    connect(_ui->actionSettings, SIGNAL(triggered()), this, SLOT(ShowSettings()));
    connect(_ui->actionStatistics, SIGNAL(triggered()), this, SLOT(ShowStatistics()));
    connect(_ui->actionExit, SIGNAL(triggered()), this, SLOT(exit()));

//    restoreAction = new QAction(tr("&Show MainWindow"), this);
//    connect(restoreAction, &QAction::triggered, this, &QWidget::showNormal);

    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(_ui->actionStartStopBroadcast);
    trayIconMenu->addAction(_ui->actionPreview);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(_ui->actionStatistics);
    trayIconMenu->addAction(_ui->actionSettings);
    trayIconMenu->addSeparator();
    //trayIconMenu->addAction(restoreAction);
    trayIconMenu->addAction(_ui->actionExit);

    auto icon = QIcon(":/Images/SysTray/icon.svg");
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(icon);
    this->setWindowIcon(icon);
    trayIcon->show();
}


void MainWindow::SetVisible(bool visible)
{
    this->setVisible(visible);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (trayIcon->isVisible()) {
          ShowTrayMessage("AmbiColor","Minimized to tray", QSystemTrayIcon::MessageIcon::Information);
        hide();
        event->ignore();
    }
}

void MainWindow::ShowTrayMessage(QString title, QString message, QSystemTrayIcon::MessageIcon icon)
{
    trayIcon->showMessage(title, message, icon, 5000);
}

void MainWindow::ShowSettings()
{
    auto result = _settingsDialog->exec();
    if(result == 1)
    {
        _framegrabber.StopCapture();
        _framegrabber.StartCapture();
    }
}

void MainWindow::ShowStatistics()
{
    _statisticsDialog->show();
}

void MainWindow::ShowPreview(){
    _previewDialog->show();
}

void MainWindow::ExitApp()
{
    exit();
}

void MainWindow::ToggleBroadCast(){
    const char* text;
    if(_teensyThreader.IsRunning()){
        text = "Stopped Broadcast";
        _teensyThreader.StopBroadcast();
        _connector->CloseDevice();
        _ui->actionStartStopBroadcast->setText("Start Cast");
    }
    else {
        if(_connector->OpenDevice() && _connector->ConfigureDevice()){
            _teensyThreader.StartBroadcast();
            _ui->actionStartStopBroadcast->setText("Stop Cast");
            text = "Started Broadcast";
        }
        else
            text = "Failed to start broadcast";

    }

    ShowTrayMessage("AmbiColor",text, QSystemTrayIcon::MessageIcon::Information);
    _ui->statusbar->showMessage(text);
}

void MainWindow::exit()
{
    _teensyThreader.StopBroadcast();
    _framegrabber.StopCapture();
    _connector->CloseDevice();
    QCoreApplication::quit();
}

MainWindow::~MainWindow()
{
    delete _ui;
}
