#include "MainLayout.hpp"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPointer>
#include <QProgressBar>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QTimer>
#include <QGroupBox>

#include "ffdropenc.hpp"

MainLayout::MainLayout(QWidget* parent) : QWidget(parent)
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

    setLayout(layout);
}