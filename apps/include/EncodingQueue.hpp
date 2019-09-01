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
    enum class Status { Stopped, Running };
    EncodingQueue();

    void insert(
        std::vector<std::filesystem::path> files,
        const ffdropenc::EncodeSettings& s);

public slots:
    void startQueue();
    void stopQueue();
    void onEncodeStart();
    void onEncodeUpdateOut();
    void onEncodeUpdateErr();
    void onEncodeFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onEncodeError(QProcess::ProcessError error);

signals:
    void queueRunning();
    void queueStopped();
    void progressUpdated(float percent);
    void newShortMessage(const QString& msg);
    void newDetailMessage(const QString& msg);

protected:
    std::vector<ffdropenc::QueueItem::Pointer> queue_;
    QPointer<QProcess> encoder_;
    ffdropenc::QueueItem::Pointer encoderCurrentItem_;

    Status status_{Status::Stopped};

    void advance_queue_();
    void eject_current_item_();
    void eject_all_items_();
};