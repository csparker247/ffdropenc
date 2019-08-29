#pragma once

#include <filesystem>
#include <vector>

#include <QObject>
#include <QPointer>
#include <QProcess>

#include "ffdropenc/EncodeSettings.hpp"
#include "ffdropenc/QueueItem.hpp"

class EncodingQueue : public QObject
{
    Q_OBJECT
public:
    EncodingQueue();

    void insert(
        std::vector<std::filesystem::path> files,
        const ffdropenc::EncodeSettings& s);
    void start();
    void stop();

public slots:
    void onAnalyzeStart();
    void onAnalyzeUpdateOut();
    void onAnalyzeUpdateErr();
    void onAnalyzeFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onAnalyzeError(QProcess::ProcessError error);

    void onEncodeStart();
    void onEncodeUpdateOut();
    void onEncodeUpdateErr();
    void onEncodeFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onEncodeError(QProcess::ProcessError error);

signals:
    void encodeStarted();
    void encodeCompleted();
    void analysisStarted();
    void analysisCompleted();
    void queueRunning();
    void queueStopped();
    void progressUpdated(float percent);

protected:
    std::vector<ffdropenc::QueueItem::Pointer> queue_;

    QPointer<QProcess> analyzer_;
    ffdropenc::QueueItem::Pointer analyzerCurrentItem_;
    QPointer<QProcess> encoder_;
    ffdropenc::QueueItem::Pointer encoderCurrentItem_;

    void start_or_advance_queue_();
};