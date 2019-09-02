#include "MainLayout.hpp"

#include <deque>

#include <QCoreApplication>
#include <QDateTime>
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

#include "CollapsibleGroupBox.hpp"
#include "ffdropenc.hpp"
#include "ffdropenc/EncodeSettings.hpp"
#include "ffdropenc/Filesystem.hpp"
#include "ffdropenc/Preset.hpp"
#include "ffdropenc/QueueItem.hpp"

using namespace ffdropenc;
namespace fs = std::filesystem;

QMap<QString, Preset::Pointer> PRESETS;
QStringList PRESET_NAMES;
QString READY_MESSAGE{"Drag files to window to begin..."};
QString DATETIME_FMT{"[MM/dd/yy hh:mm:ss] "};

MainLayout::MainLayout(QWidget* parent) : QMainWindow(parent)
{
    setAcceptDrops(true);

    QPointer<QVBoxLayout> layout = new QVBoxLayout();
    layout->setAlignment(Qt::AlignTop);
    // Label
    shortLabel_ = new QLabel(READY_MESSAGE);
    layout->addWidget(shortLabel_);

    // Progress bar
    QPointer<QWidget> info = new QWidget();
    info->setLayout(new QHBoxLayout());
    info->layout()->setMargin(0);
    progressBar_ = new QProgressBar();
    progressBar_->setMinimum(0);
    progressBar_->setMaximum(0);
    info->layout()->addWidget(progressBar_);

    cancelBtn_ = new QPushButton("Cancel");
    cancelBtn_->setEnabled(false);
    info->layout()->addWidget(cancelBtn_);
    connect(
        cancelBtn_, &QPushButton::clicked, &queue_, &EncodingQueue::stopQueue);

    layout->addWidget(info);

    // Details block
    auto collapseBox = new CollapsibleGroupBox();
    collapseBox->setContentLayout(new QVBoxLayout());
    collapseBox->contentLayout()->setMargin(0);
    details_ = new QTextEdit();
    details_->setReadOnly(true);
    QString appname = "ffdropenc v";
    appname.append(FFDROPENC_VER);
    details_->append(appname);
    details_->append(QString("-").repeated(appname.size()));
    collapseBox->contentLayout()->addWidget(details_);

    layout->addWidget(collapseBox);

    setCentralWidget(new QWidget());
    centralWidget()->setLayout(layout);

    // Settings dialog
    settings_ = new SettingsDialog(this);

    connect(this, &MainLayout::filesDropped, this, &MainLayout::processFiles);
    connect(
        &queue_, &EncodingQueue::queueRunning, this,
        &MainLayout::encodingStarted);
    connect(
        &queue_, &EncodingQueue::queueStopped, this, &MainLayout::encodingDone);
    connect(
        &queue_, &EncodingQueue::progressUpdated, this,
        &MainLayout::updateProgress);
    connect(
        &queue_, &EncodingQueue::newShortMessage, this,
        &MainLayout::shortMessage);
    connect(
        &queue_, &EncodingQueue::newDetailMessage, this,
        &MainLayout::detailMessage);

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
    emit filesDropped(paths);
}

void MainLayout::processFiles(std::vector<fs::path> files)
{
    // Select the preset
    auto result = settings_->exec();
    if (result == QDialog::Rejected) {
        return;
    }

    // Get encode settings
    EncodeSettings settings;
    settings.preset = PRESETS[settings_->getPreset()];
    settings.inputFPS = settings_->getInputFPS();
    settings.outputFPS = settings_->getOutputFPS();
    settings.outputDir = settings_->getOutputDir().toStdString();

    queue_.insert(std::move(files), settings);
}

void MainLayout::encodingStarted()
{
    progressBar_->setMaximum(100);
    cancelBtn_->setEnabled(true);
}

void MainLayout::encodingDone()
{
    progressBar_->setMaximum(0);
    shortLabel_->setText(READY_MESSAGE);
    cancelBtn_->setEnabled(false);
}

void MainLayout::updateProgress(float percent)
{
    qDebug() << "Progress: " << static_cast<int>(std::floor(percent));
    progressBar_->setValue(static_cast<int>(std::floor(percent)));
}

void MainLayout::shortMessage(const QString& msg) { shortLabel_->setText(msg); }

void MainLayout::detailMessage(const QString& msg)
{
    details_->append(QDateTime::currentDateTime().toString(DATETIME_FMT) + msg);
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
