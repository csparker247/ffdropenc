#include "MainLayout.hpp"

#include <QDebug>
#include <QDirIterator>
#include <QDropEvent>
#include <QHBoxLayout>
#include <QInputDialog>
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
#include "ffdropenc/Preset.hpp"
#include "ffdropenc/QueueItem.hpp"

using namespace ffdropenc;

QMap<QString, Preset::Pointer> PRESETS;

MainLayout::MainLayout(QWidget* parent) : QMainWindow(parent)
{
    setAcceptDrops(true);

    QPointer<QVBoxLayout> layout = new QVBoxLayout();
    layout->setAlignment(Qt::AlignTop);
    // Label
    QPointer<QLabel> shortLabel = new QLabel("Drag files on window to begin...");
    layout->addWidget(shortLabel);

    // Progress bar
    QPointer<QWidget> info = new QWidget();
    info->setLayout(new QHBoxLayout());
    info->layout()->setMargin(0);
    QPointer<QProgressBar> progressBar = new QProgressBar();
    progressBar->setMinimum(0);
    progressBar->setMaximum(0);
    info->layout()->addWidget(progressBar);

    QPointer<QPushButton> cancelBtn = new QPushButton("Cancel");
    cancelBtn->setEnabled(false);
    info->layout()->addWidget(cancelBtn);

    layout->addWidget(info);

    // Details block
    QPointer<QTextEdit> details = new QTextEdit();
    details->setReadOnly(true);
    QString appname = "ffdropenc v";
    appname.append(FFDROPENC_VER);
    details->append(QString("=").repeated(appname.size()));
    details->append(appname);
    details->append(QString("=").repeated(appname.size()));
    layout->addWidget(details);

    setCentralWidget(new QWidget());
    centralWidget()->setLayout(layout);

    // load presets
    QDirIterator it(":/presets/");
    while (it.hasNext()) {
        it.next();
        auto preset = Preset::New(it.filePath());
        PRESETS[preset->getListName()] = preset;
    }
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
    const QMimeData* mimeData = event->mimeData();

    // check for our needed mime type, here a file or a list of files
    QStringList pathList;
    if (mimeData->hasUrls()) {
        QList<QUrl> urlList = mimeData->urls();

        // extract the local paths of the files
        for (const auto& url : urlList) {
            pathList.append(url.toLocalFile());
        }
    }

    event->accept();
    // Process files
    processFiles(pathList);
}

void MainLayout::processFiles(const QStringList& files)
{
    // Get list of presets
    QStringList presetNames;
    for (const auto& p : PRESETS) {
        presetNames.push_back(p->getListName());
    }

    //
    Preset::Pointer preset;
    bool ok;
    auto item = QInputDialog::getItem(
        this, "ffdropenc", "Select output preset:", presetNames, 0, false, &ok);
    if (ok && !item.isEmpty()) {
        preset = PRESETS[item];
    }

    // Process list
    QList<QueueItem> fileList;
    for (const auto& f : files) {
        fileList.push_back(QueueItem(f, preset));
    }

    for (const auto& f : fileList) {
        qDebug() << QString::fromStdString(f.inputPath().string());
    }
}