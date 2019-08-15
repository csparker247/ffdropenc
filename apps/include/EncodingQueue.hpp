#pragma once

#include <deque>
#include <vector>
#include <filesystem>

#include <QObject>
#include <QProcess>
#include <QPointer>

#include "ffdropenc/EncodeSettings.hpp"
#include "ffdropenc/QueueItem.hpp"

class EncodingQueue : public QObject
{
    Q_OBJECT
public:
    EncodingQueue();

    void insert(std::vector<std::filesystem::path> files, const ffdropenc::EncodeSettings& s);
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
    void progressUpdated();
    void encodeStarted();
    void encodeCompleted();
    void analysisStarted();
    void analysisCompleted();
    void allStopped();

protected:
    std::deque<ffdropenc::QueueItem> analysisQueue_;
    std::deque<ffdropenc::QueueItem> encodeQueue_;

    QPointer<QProcess> analyzer_;
    QPointer<QProcess> encoder_;

    void start_or_advance_queue_();
};