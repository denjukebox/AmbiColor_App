#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QAbstractButton>
#include "Util/settings.h"

using namespace AC;

namespace Ui {
    class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

    double CalculateRatio();
private slots:
    void SaveSettings(QAbstractButton *button);

private:
    Ui::SettingsDialog *_ui;
    Settings *_settings = &Settings::Instance();
    const QStringList ratios = { "Native", "16:9", "21:9", "4:3" };
};



#endif // SETTINGSDIALOG_H
