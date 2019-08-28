#include "EncodingQueue.hpp"

#include <algorithm>

#include "ffdropenc/Filesystem.hpp"

#include <QCoreApplication>
#include <QDebug>

using namespace ffdropenc;

namespace fs = std::filesystem;

EncodingQueue::EncodingQueue()
{
    // Setup analyzer
    analyzer_ = new QProcess();
    analyzer_->setWorkingDirectory(QCoreApplication::applicationDirPath());
    analyzer_->setProgram("sleep");
    connect(
        analyzer_, &QProcess::started, this, &EncodingQueue::onAnalyzeStart);
    connect(
        analyzer_, &QProcess::readyReadStandardOutput, this,
        &EncodingQueue::onAnalyzeUpdateOut);
    connect(
        analyzer_, &QProcess::readyReadStandardError, this,
        &EncodingQueue::onAnalyzeUpdateErr);
    connect(
        analyzer_,
        QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
        &EncodingQueue::onAnalyzeFinished);
    connect(
        analyzer_, &QProcess::errorOccurred, this,
        &EncodingQueue::onAnalyzeError);

    // Setup encoder
    encoder_ = new QProcess();
    encoder_->setWorkingDirectory(QCoreApplication::applicationDirPath());
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
    analyzer_->kill();
    analyzerCurrentItem_->setStatus(QueueItem::Status::ReadyAnalysis);
    analyzerCurrentItem_ = nullptr;

    encoder_->kill();
    encoderCurrentItem_->setStatus(QueueItem::Status::ReadyEncode);
    encoderCurrentItem_ = nullptr;
    emit(allStopped());
}

void EncodingQueue::insert(std::vector<fs::path> files, const EncodeSettings& s)
{
    // Expand directories and filter bad files
    files = FilterFileList(files);

    // Create temp queue
    std::vector<QueueItem::Pointer> tempQueue;
    for (const auto& f : files) {
        try {
            tempQueue.emplace_back(QueueItem::New(f, s.preset));
        } catch (const std::runtime_error& e) {
            qDebug() << e.what() << ":" << f.c_str();
        }
    }

    // Sort queue by name and remove duplicates
    std::sort(tempQueue.begin(), tempQueue.end());
    auto last = std::unique(tempQueue.begin(), tempQueue.end());
    tempQueue.erase(last, tempQueue.end());

    // insert items into analysis queue
    queue_.insert(queue_.end(), tempQueue.begin(), tempQueue.end());

    // Start processing the queue
    start_or_advance_queue_();
}

void EncodingQueue::onAnalyzeStart()
{
    qDebug() << "Analysis started:"
             << analyzerCurrentItem_->inputPath().stem().c_str();
    // progressBar->setMaximum(100);
}

void EncodingQueue::onAnalyzeUpdateOut()
{
    qDebug() << "Analyze Out:" << analyzer_->readAllStandardOutput();
}

void EncodingQueue::onAnalyzeUpdateErr()
{
    qDebug() << "Analyze Err:" << analyzer_->readAllStandardError();
}

void EncodingQueue::onAnalyzeFinished(
    int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "Analyze Finished" << exitCode << exitStatus;
    // progressBar->setMaximum(0);

    // Update status
    analyzerCurrentItem_->setStatus(QueueItem::Status::ReadyEncode);
    analyzerCurrentItem_ = nullptr;
    start_or_advance_queue_();
}

void EncodingQueue::onAnalyzeError(QProcess::ProcessError error)
{
    qDebug() << "Analyze Error" << error;
}

void EncodingQueue::onEncodeStart()
{
    qDebug() << "Encode started:"
             << encoderCurrentItem_->inputPath().stem().c_str();
    // progressBar->setMaximum(100);
}

void EncodingQueue::onEncodeUpdateOut()
{
    qDebug() << "Encode Out:" << encoder_->readAllStandardOutput();
}

void EncodingQueue::onEncodeUpdateErr()
{
    qDebug() << "Encode Err:" << encoder_->readAllStandardError();
}

void EncodingQueue::onEncodeFinished(
    int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "Encode Finished" << exitCode << exitStatus;
    // progressBar->setMaximum(0);
    encoderCurrentItem_->setStatus(QueueItem::Status::Done);
    queue_.erase(
        std::remove(queue_.begin(), queue_.end(), encoderCurrentItem_),
        queue_.end());
    encoderCurrentItem_ = nullptr;
    start_or_advance_queue_();
}

void EncodingQueue::onEncodeError(QProcess::ProcessError error)
{
    qDebug() << "Error" << error;
}

void EncodingQueue::start_or_advance_queue_()
{
    // Advance the analysis queue
    if (!queue_.empty() && analyzer_->state() == QProcess::NotRunning) {
        for (auto& q : queue_) {
            if (q->status() != QueueItem::Status::ReadyAnalysis) {
                continue;
            }

            analyzerCurrentItem_ = q;
            analyzer_->setArguments({"5"});
            analyzer_->start();
            emit(analysisStarted());
        }
    }

    // Advance the encoder queue
    if (!queue_.empty() && encoder_->state() == QProcess::NotRunning) {
        for (auto& q : queue_) {
            if (q->status() != QueueItem::Status::ReadyEncode) {
                continue;
            }
            encoderCurrentItem_ = q;
            encoder_->setArguments(q->encodeArguments());
            encoder_->start();
            emit(encodeStarted());
        }
    }
}