#include "MainLayout.hpp"

#include <QDebug>
#include <QDropEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QMimeData>
#include <QPointer>
#include <QProgressBar>
#include <QPushButton>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>

#include "ffdropenc.hpp"

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
    if (mimeData->hasUrls()) {
        QStringList pathList;
        QList<QUrl> urlList = mimeData->urls();

        // extract the local paths of the files
        for (const auto& url : urlList) {
            pathList.append(url.toLocalFile());
            qDebug() << url.toLocalFile();
        }

        // call a function to open the files
        // openFiles(pathList);
    }
}