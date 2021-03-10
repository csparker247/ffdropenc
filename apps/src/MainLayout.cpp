#include "MainLayout.hpp"

#include <deque>

#include <QDateTime>
#include <QDebug>
#include <QDirIterator>
#include <QDropEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QList>
#include <QMap>
#include <QMenuBar>
#include <QMessageBox>
#include <QMimeData>
#include <QPointer>
#include <QProgressBar>
#include <QPushButton>
#include <QSettings>
#include <QTextEdit>
#include <QVBoxLayout>

#include "CollapsibleGroupBox.hpp"
#include "MainApplication.hpp"
#include "ffdropenc.hpp"
#include "ffdropenc/EncodeSettings.hpp"
#include "ffdropenc/Preset.hpp"

using namespace ffdropenc;
namespace fs = std::filesystem;

QMap<QString, Preset::Pointer> PRESETS;
QStringList PRESET_NAMES;
QString READY_MESSAGE{"Drag files to window to begin..."};
QString DATETIME_FMT{"[MM/dd/yy hh:mm:ss] "};

MainLayout::MainLayout(QWidget* parent) : QMainWindow(parent)
{
    setAcceptDrops(true);
    setWindowIcon(QIcon{":/logo"});
    setWindowTitle("ffdropenc");
    setMinimumWidth(480);

    QPointer<QVBoxLayout> layout = new QVBoxLayout();
    layout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    // Settings
    auto fileMenu = this->menuBar()->addMenu(" &File");
    settingsWindow_ = new SettingsWindow();
    queue_.setExecutablePath(settingsWindow_->currentExecutablePath());

    QString appname = "ffdropenc v";
    appname.append(FFDROPENC_VER);
    auto aboutAction = new QAction("&About", this);
    connect(aboutAction, &QAction::triggered, [=]() {
        QMessageBox::about(this, "ffdropenc", appname);
    });
    fileMenu->addAction(aboutAction);

    auto prefsAction = new QAction("&Preferences", this);
    prefsAction->setStatusTip("Open the preferences menu");
    connect(prefsAction, &QAction::triggered, settingsWindow_, &QDialog::show);
    fileMenu->addAction(prefsAction);

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
    collapseBox_ = new CollapsibleGroupBox();
    collapseBox_->setContentLayout(new QVBoxLayout());
    collapseBox_->contentLayout()->setMargin(0);
    collapseBox_->setTitle("Details");

    // Ensures the window gets resized on collapse
    connect(collapseBox_, &CollapsibleGroupBox::toggled, this, [this]() {
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        setMinimumHeight(minimumSizeHint().height());
        resize(size().width(), sizeHint().height());
    });

    details_ = new QTextEdit();
    details_->setReadOnly(true);
    details_->append("<font color=\"Gray\">" + appname + "</font>");
    details_->append(
        "<font color=\"Gray\">" + QString("-").repeated(appname.size()) +
        "</font>");
    collapseBox_->contentLayout()->addWidget(details_);

    layout->addWidget(collapseBox_);

    setCentralWidget(new QWidget());
    centralWidget()->setLayout(layout);

    // Settings dialog
    settings_ = new SettingsDialog(this);

    // connect
    connect(
        dynamic_cast<MainApplication*>(MainApplication::instance()),
        &MainApplication::filesDropped, this, &MainLayout::processFiles);

    connect(this, &MainLayout::filesDropped, this, &MainLayout::processFiles);
    connect(
        settingsWindow_, &SettingsWindow::ExecutableChanged, &queue_,
        &EncodingQueue::setExecutablePath);
    connect(
        settingsWindow_, &SettingsWindow::ExecutableChanged, this,
        &MainLayout::on_executable_changed_);
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
        &queue_, &EncodingQueue::newDetailInfo, this,
        &MainLayout::detailMessage);
    connect(
        &queue_, &EncodingQueue::newDetailError, this,
        &MainLayout::errorMessage);
    connect(
        &queue_, &EncodingQueue::newDetailSuccess, this,
        &MainLayout::successMessage);

    // load settings
    load_settings_();
    // load presets
    load_presets_();
}

void MainLayout::save_settings_()
{
    QSettings settings;
    settings.setValue("window/geometry", saveGeometry());
    settings.setValue("window/state", saveState());
    settings.setValue("window/expandDetails", collapseBox_->isExpanded());
}

void MainLayout::load_settings_()
{
    QSettings settings;

    // Restore this first because it causes a window resize
    collapseBox_->setExpanded(
        settings.value("window/expandDetails", false).toBool());

    // Restore window geometry and state
    if (settings.contains("window/geometry")) {
        restoreGeometry(settings.value("window/geometry").toByteArray());
    }
    if (settings.contains("window/state")) {
        restoreState(settings.value("window/state").toByteArray());
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

void MainLayout::closeEvent(QCloseEvent* event)
{
    save_settings_();
    event->accept();
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
    QString m("<font color=\"Gray\">");
    m.append(QDateTime::currentDateTime().toString(DATETIME_FMT));
    m.append(msg);
    m.append("</font>");
    details_->append(m);
}

void MainLayout::errorMessage(const QString& msg)
{
    QString m("<font color=\"Red\">");
    m.append(QDateTime::currentDateTime().toString(DATETIME_FMT) + msg);
    m.append("</font>");
    details_->append(m);
}

void MainLayout::successMessage(const QString& msg)
{
    QString m("<font color=\"Green\">");
    m.append(QDateTime::currentDateTime().toString(DATETIME_FMT) + msg);
    m.append("</font>");
    details_->append(m);
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

void MainLayout::on_executable_changed_(const QString& path)
{
    // TODO: Reports full path for bundled
    // detailMessage("Executable changed: " + path);
}
