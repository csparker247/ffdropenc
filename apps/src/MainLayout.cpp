#include "MainLayout.hpp"

#include <deque>

#include <QCoreApplication>
#include <QDebug>
#include <QDirIterator>
#include <QDropEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QList>
#include <QMap>
#include <QMimeData>
#include <QPointer>
#include <QProgressBar>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

#include "ffdropenc.hpp"
#include "ffdropenc/Filesystem.hpp"
#include "ffdropenc/Preset.hpp"
#include "ffdropenc/QueueItem.hpp"

using namespace ffdropenc;
namespace fs = std::filesystem;

QMap<QString, Preset::Pointer> PRESETS;
QStringList PRESET_NAMES;
std::deque<QueueItem> QUEUE;

MainLayout::MainLayout(QWidget* parent) : QMainWindow(parent)
{
    setAcceptDrops(true);

    QPointer<QVBoxLayout> layout = new QVBoxLayout();
    layout->setAlignment(Qt::AlignTop);
    // Label
    shortLabel = new QLabel("Drag files to window to begin...");
    layout->addWidget(shortLabel);

    // Progress bar
    QPointer<QWidget> info = new QWidget();
    info->setLayout(new QHBoxLayout());
    info->layout()->setMargin(0);
    progressBar = new QProgressBar();
    progressBar->setMinimum(0);
    progressBar->setMaximum(0);
    info->layout()->addWidget(progressBar);

    cancelBtn = new QPushButton("Cancel");
    cancelBtn->setEnabled(false);
    info->layout()->addWidget(cancelBtn);

    layout->addWidget(info);

    // Details block
    details = new QTextEdit();
    details->setReadOnly(true);
    QString appname = "ffdropenc v";
    appname.append(FFDROPENC_VER);
    details->append(appname);
    details->append(QString("-").repeated(appname.size()));
    layout->addWidget(details);

    setCentralWidget(new QWidget());
    centralWidget()->setLayout(layout);

    // Settings dialog
    settings_ = new SettingsDialog(this);

    // Setup process
    ffmpeg = new QProcess();
    ffmpeg->setWorkingDirectory(QCoreApplication::applicationDirPath());
    ffmpeg->setProgram("ffmpeg");
    connect(ffmpeg, &QProcess::started, this, &MainLayout::onTranscodeStart);
    connect(
        ffmpeg, &QProcess::readyReadStandardOutput, this,
        &MainLayout::onTranscodeUpdateOut);
    connect(
        ffmpeg, &QProcess::readyReadStandardError, this,
        &MainLayout::onTranscodeUpdateErr);
    connect(
        ffmpeg, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        this, &MainLayout::onTranscodeFinished);
    connect(
        ffmpeg, &QProcess::errorOccurred, this, &MainLayout::onTranscodeError);

    connect(this, &MainLayout::filesDropped, this, &MainLayout::processFiles);

    // load presets
    load_presets_();
}

void MainLayout::dragEnterEvent(QDragEnterEvent* event)
{
    event->acceptProposedAction();
}

void MainLayout::dragMoveEvent(QDragMoveEvent* event)
{
    event->acceptProposedAction();
}

void MainLayout::dragLeaveEvent(QDragLeaveEvent* event) { event->accept(); }

void MainLayout::dropEvent(QDropEvent* event)
{
    // check for our needed mime type, here a file or a list of files
    const QMimeData* mimeData = event->mimeData();
    std::vector<fs::path> paths;
    if (mimeData->hasUrls()) {
        QList<QUrl> urlList = mimeData->urls();

        // extract the local paths of the files
        for (const auto& url : urlList) {
            paths.emplace_back(url.toLocalFile().toStdString());
        }
    }

    event->accept();
    emit(filesDropped(paths));
}

void MainLayout::processFiles(std::vector<fs::path> files)
{
    // Expand directories and filter bad files
    files = ffdropenc::FilterFileList(files);

    // Select the preset
    auto result = settings_->exec();
    if (result == QDialog::Rejected) {
        return;
    }

    // Preset
    auto preset = PRESETS[settings_->getPreset()];

    // Create temp queue
    std::vector<QueueItem> tempQueue;
    for (const auto& f : files) {
        try {
            tempQueue.emplace_back(f, preset);
        } catch (const std::runtime_error& e) {
            qDebug() << e.what() << ":" << f.c_str();
        }
    }

    // Sort queue by name and remove duplicates
    std::sort(tempQueue.begin(), tempQueue.end());
    auto duplicateRemover = std::unique(tempQueue.begin(), tempQueue.end());
    tempQueue.resize(std::distance(tempQueue.begin(), duplicateRemover));

    // insert items into global queue
    QUEUE.insert(QUEUE.end(), tempQueue.begin(), tempQueue.end());

    // Start processing the queue
    start_or_advance_queue_();
}

void MainLayout::load_presets_()
{
    // Empty the current lists
    PRESETS.clear();
    PRESET_NAMES.clear();

    // Scan the embedded directory
    QDirIterator it(":/presets/");
    while (it.hasNext()) {
        it.next();
        auto preset = Preset::New(it.filePath());
        PRESETS[preset->getListName()] = preset;
        PRESET_NAMES.push_back(preset->getListName());
    }

    std::sort(PRESET_NAMES.begin(), PRESET_NAMES.end());

    settings_->setPresetList(PRESET_NAMES);
}

void MainLayout::onTranscodeStart()
{
    qDebug() << "Started";
    progressBar->setMaximum(100);
}

void MainLayout::onTranscodeUpdateOut()
{
    qDebug() << "Out" << ffmpeg->readAllStandardOutput();
}

void MainLayout::onTranscodeUpdateErr()
{
    qDebug() << "Err" << ffmpeg->readAllStandardError();
}

void MainLayout::onTranscodeFinished(
    int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "Finished" << exitCode << exitStatus;
    progressBar->setMaximum(0);
    start_or_advance_queue_();
}

void MainLayout::onTranscodeError(QProcess::ProcessError error)
{
    qDebug() << "Error" << error;
}

void MainLayout::start_or_advance_queue_()
{
    if (!QUEUE.empty() && ffmpeg->state() == QProcess::NotRunning) {
        auto q = QUEUE.front();
        QUEUE.pop_front();

        progressBar->setMaximum(100);
        shortLabel->setText(
            "Transcoding " + q.preset()->getConsoleName() + " version of " +
            q.inputPath().stem().c_str() + "...");

        ffmpeg->setArguments(q.encodeArguments());
        ffmpeg->start();
    } else {
        shortLabel->setText("Drag files to window to begin...");
    }
}
