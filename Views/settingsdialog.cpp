#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <cstdlib>
#include <climits>
#include <string>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    _ui(new Ui::SettingsDialog)
{
    _ui->setupUi(this);
    _ui->widthEdit->setText(QString::number(_settings->GetWidth()));
    _ui->heightEdit->setText(QString::number(_settings->GetHeight()));
    _ui->depthEdit->setText(QString::number(_settings->GetDepth()));
    _ui->brightnessEdit->setText(QString::number(_settings->GetBrightness()));
    _ui->captureEdit->setText(QString::number(1000 / _settings->GetCaptureRate().count()));
    _ui->teensyEdit->setText(QString::number(1000 / _settings->GetBroadcastRate().count()));
    _ui->ratioCombo->addItems(ratios);

    connect(_ui->buttonBox, &QDialogButtonBox::clicked, this, &SettingsDialog::SaveSettings);
}

void SettingsDialog::SaveSettings(QAbstractButton *button)
{
    if(button == ((QAbstractButton *)_ui->buttonBox->button(QDialogButtonBox::Save))){
        _settings->SetWidth(_ui->widthEdit->text().toInt());
        _settings->SetHeight(_ui->heightEdit->text().toInt());
        _settings->SetDepth(_ui->depthEdit->text().toInt());
        _settings->SetBrightness(_ui->brightnessEdit->text().toInt());
        _settings->SetCaptureRate(std::chrono::milliseconds(_ui->captureEdit->text().toInt() * 1000));
        _settings->SetBroadcastRate(std::chrono::milliseconds(_ui->teensyEdit->text().toInt() * 1000));
        _settings->SetContentRatio(CalculateRatio());
    }
    this->close();
}

double SettingsDialog::CalculateRatio()
{
    auto ratioText = _ui->ratioCombo->currentText();
    if(ratioText.contains(':')){
        auto split = ratioText.split(":");
        auto horizontal = split[0].toDouble();
        auto vertical = split[1].toDouble();
        return vertical / horizontal;
    }

    return 0.0;
}

SettingsDialog::~SettingsDialog()
{
    delete _ui;
}
