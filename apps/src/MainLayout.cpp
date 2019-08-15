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
#include "ffdropenc/EncodeSettings.hpp"
#include "ffdropenc/Filesystem.hpp"
#include "ffdropenc/Preset.hpp"
#include "ffdropenc/QueueItem.hpp"
#include "EncodingQueue.hpp"

using namespace ffdropenc;
namespace fs = std::filesystem;

QMap<QString, Preset::Pointer> PRESETS;
QStringList PRESET_NAMES;
EncodingQueue QUEUE;

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
    // Select the preset
    auto result = settings_->exec();
    if (result == QDialog::Rejected) {
        return;
    }

    // Get encode settings
    EncodeSettings settings;
    settings.preset = PRESETS[settings_->getPreset()];

    QUEUE.insert(std::move(files), settings);
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
