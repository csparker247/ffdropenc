#pragma once

#include <filesystem>

#include <QLabel>
#include <QMainWindow>
#include <QPointer>
#include <QProgressBar>
#include <QPushButton>
#include <QStringList>
#include <QTextEdit>

#include "EncodingQueue.hpp"
#include "SettingsDialog.hpp"
#include "SettingsWindow.hpp"

class MainLayout : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainLayout(QWidget* parent = nullptr);

    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dragLeaveEvent(QDragLeaveEvent* event) override;
    void dropEvent(QDropEvent* event) override;

signals:
    void filesDropped(std::vector<std::filesystem::path>);

public slots:
    void processFiles(std::vector<std::filesystem::path> files);
    void encodingStarted();
    void encodingDone();
    void updateProgress(float percent);
    void shortMessage(const QString& msg);
    void detailMessage(const QString& msg);
    void errorMessage(const QString& msg);
    void successMessage(const QString& msg);

protected:
    QPointer<SettingsWindow> settingsWindow_;
    QPointer<QLabel> shortLabel_;
    QPointer<QProgressBar> progressBar_;
    QPointer<QPushButton> cancelBtn_;
    QPointer<QTextEdit> details_;
    QPointer<SettingsDialog> settings_;

    EncodingQueue queue_;

    void load_presets_();
};