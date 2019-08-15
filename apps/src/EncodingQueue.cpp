#include "EncodingQueue.hpp"

#include "ffdropenc/Filesystem.hpp"

#include <QDebug>
#include <QCoreApplication>

using namespace ffdropenc;

namespace fs = std::filesystem;

EncodingQueue::EncodingQueue()
{
    // Setup analyzer
    analyzer_ = new QProcess();
    analyzer_->setWorkingDirectory(QCoreApplication::applicationDirPath());
    analyzer_->setProgram("ffprobe");
    connect(analyzer_, &QProcess::started, this, &EncodingQueue::onAnalyzeStart);
    connect(
        analyzer_, &QProcess::readyReadStandardOutput, this,
        &EncodingQueue::onAnalyzeUpdateOut);
    connect(
        analyzer_, &QProcess::readyReadStandardError, this,
        &EncodingQueue::onAnalyzeUpdateErr);
    connect(
        analyzer_, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        this, &EncodingQueue::onAnalyzeFinished);
    connect(
        analyzer_, &QProcess::errorOccurred, this, &EncodingQueue::onAnalyzeError);

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
        encoder_, &QProcess::errorOccurred, this, &EncodingQueue::onEncodeError);
}

void EncodingQueue::start() { start_or_advance_queue_(); }

void EncodingQueue::stop() {
    analyzer_->kill();
    encoder_->kill();
    emit(allStopped());
}

void EncodingQueue::insert(std::vector<fs::path> files, const EncodeSettings& s)
{
    // Expand directories and filter bad files
    files = FilterFileList(files);

    // Create temp queue
    std::vector<QueueItem> tempQueue;
    for (const auto& f : files) {
        try {
            tempQueue.emplace_back(f, s.preset);
        } catch (const std::runtime_error& e) {
            qDebug() << e.what() << ":" << f.c_str();
        }
    }

    // Sort queue by name and remove duplicates
    std::sort(tempQueue.begin(), tempQueue.end());
    auto duplicateRemover = std::unique(tempQueue.begin(), tempQueue.end());
    tempQueue.resize(std::distance(tempQueue.begin(), duplicateRemover));

    // insert items into analysis queue
    analysisQueue_.insert(analysisQueue_.end(), tempQueue.begin(), tempQueue.end());

    // Start processing the queue
    start_or_advance_queue_();
}

void EncodingQueue::onAnalyzeStart()
{
    qDebug() << "Analysis started";
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
    start_or_advance_queue_();
}

void EncodingQueue::onAnalyzeError(QProcess::ProcessError error)
{
    qDebug() << "Analyze Error" << error;
}

void EncodingQueue::onEncodeStart()
{
    qDebug() << "Encode started";
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
    start_or_advance_queue_();
}

void EncodingQueue::onEncodeError(QProcess::ProcessError error)
{
    qDebug() << "Error" << error;
}

void EncodingQueue::start_or_advance_queue_()
{
    // Advance the analysis queue
    if (!analysisQueue_.empty() && analyzer_->state() == QProcess::NotRunning) {
        auto q = analysisQueue_.front();
        analysisQueue_.pop_front();

        analyzer_->setArguments(q.encodeArguments());
        analyzer_->start();
        emit(analysisStarted());
    }

    // Advance the encoder queue
    if (!encodeQueue_.empty() && encoder_->state() == QProcess::NotRunning) {
        auto q = encodeQueue_.front();
        encodeQueue_.pop_front();

        encoder_->setArguments(q.encodeArguments());
        encoder_->start();
        emit(encodeStarted());
    }
}