#include "EncodingQueue.hpp"

#include <algorithm>

#include <QDebug>
#include <QRegExp>

#include "MainApplication.hpp"
#include "ffdropenc/Filesystem.hpp"
#include "ffdropenc/TimeTools.hpp"

using namespace ffdropenc;

namespace fs = std::filesystem;

QRegExp ENCODER_DURATION{".*Duration:\\s(\\d+:\\d+:\\d+.\\d+).*(\n|\r|\r\f)"};
QRegExp ENCODER_TIME{".*time=(\\d+:\\d+:\\d+.\\d+).*(\n|\r|\r\f)"};

EncodingQueue::EncodingQueue()
{
    // Setup encoder
    encoder_ = new QProcess();
    encoder_->setWorkingDirectory(QApplication::applicationDirPath());
    encoder_->setProgram("ffmpeg");
    connect(encoder_, &QProcess::started, this, &EncodingQueue::onEncodeStart);
    connect(
        encoder_, &QProcess::readyReadStandardOutput, this,
        &EncodingQueue::onEncodeUpdateOut);
    connect(
        encoder_, &QProcess::readyReadStandardError, this,
        &EncodingQueue::onEncodeUpdateErr);
    connect(
        encoder_, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        this, &EncodingQueue::onEncodeFinished);
    connect(
        encoder_, &QProcess::errorOccurred, this,
        &EncodingQueue::onEncodeError);
}

void EncodingQueue::start() { start_or_advance_queue_(); }

void EncodingQueue::stop()
{
    encoder_->kill();
    encoderCurrentItem_->setStatus(QueueItem::Status::ReadyEncode);
    eject_current_item_();
    emit queueStopped();
}

void EncodingQueue::insert(std::vector<fs::path> files, const EncodeSettings& s)
{
    // Expand directories and filter bad files
    files = FilterFileList(files);

    // Create temp queue
    std::vector<QueueItem::Pointer> tempQueue;
    for (const auto& f : files) {
        try {
            tempQueue.emplace_back(QueueItem::New(f, s));
        } catch (const std::runtime_error& e) {
            qDebug() << e.what() << ":" << f.c_str();
        }
    }

    // Sort queue by name and remove duplicates
    std::sort(tempQueue.begin(), tempQueue.end());
    auto last = std::unique(
        tempQueue.begin(), tempQueue.end(),
        [](const QueueItem::Pointer& l, const QueueItem::Pointer& r) {
            return *l == *r;
        });
    tempQueue.erase(last, tempQueue.end());

    // insert items into analysis queue
    queue_.insert(queue_.end(), tempQueue.begin(), tempQueue.end());

    // Start processing the queue
    start_or_advance_queue_();
}

void EncodingQueue::onEncodeStart()
{
    qDebug() << "encoder started:" << encoderCurrentItem_->inputFileName();
    QString msg{"Encoding "};
    msg.append(encoderCurrentItem_->preset()->getConsoleName());
    msg.append(" version of ");
    msg.append(encoderCurrentItem_->inputFileName());
    msg.append("...");
    emit newShortMessage(msg);
    emit newDetailMessage(msg);
    emit progressUpdated(0);
}

void EncodingQueue::onEncodeUpdateOut()
{
    qDebug() << "encoder stdout:" << encoder_->readAllStandardOutput();
}

void EncodingQueue::onEncodeUpdateErr()
{
    auto line = encoder_->readAllStandardError();
    int pos = 0;
    while ((pos = ENCODER_DURATION.indexIn(line, pos)) != -1) {
        auto duration = DurationStringToSeconds(ENCODER_DURATION.cap(1));
        encoderCurrentItem_->setDuration(duration);
        pos += ENCODER_TIME.matchedLength();
    }

    pos = 0;
    while ((pos = ENCODER_TIME.indexIn(line, pos)) != -1) {
        auto time = DurationStringToSeconds(ENCODER_TIME.cap(1));
        emit progressUpdated(time / encoderCurrentItem_->duration() * 100);
        pos += ENCODER_TIME.matchedLength();
    }
    qDebug() << "encoder stderr:" << line;
}

void EncodingQueue::onEncodeFinished(
    int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "encoder finished:" << exitCode << "-" << exitStatus;
    QString msg{"Finished encoding "};
    msg.append(encoderCurrentItem_->preset()->getConsoleName());
    msg.append(" version of ");
    msg.append(encoderCurrentItem_->inputFileName());
    emit newDetailMessage(msg);
    emit progressUpdated(100);
    encoderCurrentItem_->setStatus(QueueItem::Status::Done);
    eject_current_item_();
    start_or_advance_queue_();
}

void EncodingQueue::onEncodeError(QProcess::ProcessError error)
{
    qDebug() << "encoder error:" << error;
    encoderCurrentItem_->setStatus(QueueItem::Status::Error);
    eject_current_item_();
    // To-Do: Emit message to console
    start_or_advance_queue_();
}

void EncodingQueue::start_or_advance_queue_()
{
    if (queue_.empty()) {
        emit queueStopped();
    } else {
        emit queueRunning();
    }

    // Advance the encoder queue
    if (!queue_.empty() && encoder_->state() == QProcess::NotRunning) {
        for (auto& q : queue_) {
            if (q->status() != QueueItem::Status::ReadyEncode) {
                continue;
            }
            encoderCurrentItem_ = q;
            encoder_->setArguments(q->encodeArguments());
            qDebug() << "encoder cmd:" << encoder_->program()
                     << encoder_->arguments();
            encoder_->start();
        }
    }
}

void EncodingQueue::eject_current_item_()
{
    queue_.erase(
        std::remove(queue_.begin(), queue_.end(), encoderCurrentItem_),
        queue_.end());
    encoderCurrentItem_ = nullptr;
}