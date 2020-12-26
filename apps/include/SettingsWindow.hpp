#pragma once

#include <QComboBox>
#include <QDialog>
#include <QFileDialog>
#include <QLabel>
#include <QPointer>
#include <QWidget>

class SettingsWindow : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsWindow(
        QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

private:
    QPointer<QComboBox> execSelector_;
    QPointer<QLabel> execLabel_;
    QPointer<QFileDialog> execPicker_;

    void exec_scan_();
    void exec_test_(const QString& path);
    void exec_msg_success_(const QString& msg);
    void exec_msg_error_(const QString& msg);
};