#pragma once

#include <QComboBox>
#include <QDialog>
#include <QFileDialog>
#include <QPointer>
#include <QString>
#include <QStringList>
#include <QWidget>

class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget* parent);

    void setPresetList(const QStringList& presets);

    QString getPreset() const;

public slots:
    void onDirPicked(const QString& t);

private:
    QPointer<QComboBox> presetPicker_;
    QPointer<QComboBox> inputFPSPicker_;
    QPointer<QComboBox> outputFPSPicker_;
    QPointer<QComboBox> outputDirList_;
    QString currentDir_;
    QPointer<QFileDialog> outputDirPicker_;
};
