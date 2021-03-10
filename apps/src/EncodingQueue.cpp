#include "EncodingQueue.hpp"

#include <algorithm>

#include <QDebug>
#include <QRegularExpression>

#include "MainApplication.hpp"
#include "ffdropenc/Filesystem.hpp"
#include "ffdropenc/TimeTools.hpp"

using namespace ffdropenc;

namespace fs = std::filesystem;

QRegularExpression ENCODER_DURATION{".*Duration:\\s(\\d+:\\d+:\\d+.\\d+).*(\n|\r|\r\f)"};
QRegularExpression ENCODER_TIME{".*time=(\\d+:\\d+:\\d+.\\d+).*(\n|\r|\r\f)"};

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

EncodingQueue::~EncodingQueue()
{
    if (encoder_ and encoder_->state() != QProcess::NotRunning) {
        encoder_->terminate();
        encoder_->waitForFinished();
    }
}

void EncodingQueue::startQueue()
{
    status_ = Status::Running;
    advance_queue_();
}

void EncodingQueue::stopQueue()
{
    status_ = Status::Stopped;
    encoder_->terminate();
    emit newDetailInfo("Encoding cancelled.");
    eject_all_items_();
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
        } catch (const std::exception& e) {
            qDebug() << "QueueItem error ::" << f.c_str() << "::" << e.what();
        }
    }

    // Sort queue by name and remove duplicates
    std::sort(
        tempQueue.begin(), tempQueue.end(),
        [](const auto& l, const auto& r) { return *l < *r; });
    auto last = std::unique(
        tempQueue.begin(), tempQueue.end(),
        [](const auto& l, const auto& r) { return *l == *r; });
    tempQueue.erase(last, tempQueue.end());
    qDebug() << "Adding" << tempQueue.size() << "items to queue.";

    // insert items into analysis queue
    queue_.insert(queue_.end(), tempQueue.begin(), tempQueue.end());

    // Start processing the queue
    startQueue();
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
    emit newDetailInfo(msg);
    emit progressUpdated(0);
}

void EncodingQueue::onEncodeUpdateOut()
{
    qDebug() << "encoder stdout:" << encoder_->readAllStandardOutput();
}

void EncodingQueue::onEncodeUpdateErr()
{
    auto line = encoder_->readAllStandardError();
    qDebug() << "encoder stderr:" << line;
    if (status_ == Status::Stopped || !encoderCurrentItem_) {
        return;
    }

    auto match = ENCODER_DURATION.match(line);
    if(match.hasMatch()) {
        auto duration = DurationStringToSeconds(match.captured(1));
        encoderCurrentItem_->setDuration(duration);
    }

    match = ENCODER_TIME.match(line);
    if(match.hasMatch()) {
        auto time = DurationStringToSeconds(match.captured(1));
        emit progressUpdated(time / encoderCurrentItem_->duration() * 100);
    }
}

void EncodingQueue::onEncodeFinished(
    int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "encoder finished:" << exitCode << "-" << exitStatus;
    if (encoderCurrentItem_) {
        QString msg;
        if (exitCode != 0) {
            msg.append("Error encoding ");
        } else {
            msg.append("Finished encoding ");
        }
        msg.append(encoderCurrentItem_->preset()->getConsoleName());
        msg.append(" version of ");
        msg.append(encoderCurrentItem_->inputFileName());
        if (exitCode != 0) {
            emit newDetailError(msg);
            encoderCurrentItem_->setStatus(QueueItem::Status::Error);
        } else {
            emit newDetailSuccess(msg);
            encoderCurrentItem_->setStatus(QueueItem::Status::Done);
        }
        emit progressUpdated(100);
        eject_current_item_();
    }
    advance_queue_();
}

void EncodingQueue::onEncodeError(QProcess::ProcessError error)
{
    qDebug() << "encoder error:" << error;
    eject_current_item_();
    // TODO: Emit message to console
    advance_queue_();
}

void EncodingQueue::setExecutablePath(const QString& path)
{
    qDebug() << "executable changed: " << path;
    encoder_->setProgram(path);
}

void EncodingQueue::advance_queue_()
{
    if (queue_.empty()) {
        emit queueStopped();
    } else {
        emit queueRunning();
    }

    // Advance the encoder queue
    if (status_ == Status::Running && !queue_.empty() &&
        encoder_->state() == QProcess::NotRunning) {
        for (auto& q : queue_) {
            if (q->status() != QueueItem::Status::ReadyEncode) {
                continue;
            }
            encoderCurrentItem_ = q;
            encoder_->setArguments(q->encodeArguments());
            qDebug() << "encoder cmd:" << encoder_->program()
                     << encoder_->arguments();
            encoder_->start();
            break;
        }
    }
}

void EncodingQueue::eject_current_item_()
{
    if (encoderCurrentItem_) {
        queue_.erase(
            std::remove(queue_.begin(), queue_.end(), encoderCurrentItem_),
            queue_.end());
        encoderCurrentItem_ = nullptr;
    }
}

void EncodingQueue::eject_all_items_()
{
    queue_.clear();
    encoderCurrentItem_ = nullptr;
    emit newDetailInfo("Queue cleared.");
}
