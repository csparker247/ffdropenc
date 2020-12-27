#pragma once

#include <QCloseEvent>
#include <QComboBox>
#include <QDialog>
#include <QFileDialog>
#include <QLabel>
#include <QPointer>
#include <QPushButton>
#include <QString>
#include <QWidget>

class SettingsWindow : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsWindow(
        QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    void closeEvent(QCloseEvent*) override;
    QString currentExecutablePath() const;

signals:
    void ExecutableChanged(QString);

private:
    QPointer<QComboBox> execSelector_;
    QPointer<QLabel> execLabel_;
    QPointer<QFileDialog> execPicker_;
    QPointer<QPushButton> saveBtn_;

    void exec_scan_();
    std::tuple<bool, QString> exec_test_(const QString& path);
    void exec_msg_success_(const QString& msg);
    void exec_msg_error_(const QString& msg);
    void on_browse_exec_();
    void change_exec_(const QString& execOpt);
    QString current_exec_opt_;

    void save_settings_();
    void load_settings_();
    void close_and_save_();
    void close_and_reset_();
};