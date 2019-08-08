#pragma once

#include <filesystem>

#include <QLabel>
#include <QMainWindow>
#include <QPointer>
#include <QProcess>
#include <QProgressBar>
#include <QPushButton>
#include <QStringList>
#include <QTextEdit>

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
    void onTranscodeStart();
    void onTranscodeUpdateOut();
    void onTranscodeUpdateErr();
    void onTranscodeFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onTranscodeError(QProcess::ProcessError error);

protected:
    QPointer<QLabel> shortLabel;
    QPointer<QProgressBar> progressBar;
    QPointer<QPushButton> cancelBtn;
    QPointer<QTextEdit> details;
    QPointer<QProcess> ffmpeg;

    static void load_presets_();
    void start_or_advance_queue_();
};