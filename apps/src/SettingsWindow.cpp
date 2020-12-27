#include "SettingsWindow.hpp"

#include <filesystem>

#include <QApplication>
#include <QDebug>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QProcess>
#include <QSettings>
#include <QStandardPaths>

#include "LayoutUtils.hpp"

namespace fs = std::filesystem;

QRegExp VERSION{R"(ffmpeg\sversion\s(\d+.\d+.\d+))"};
static const QString BUNDLED = "Bundled";

static QString BundledExecPath()
{
    fs::path app{QApplication::applicationDirPath().toStdString()};
    app /= "ffmpeg";
    return app.c_str();
}

static QString ResolveExecPath(const QString& p)
{
    if (p == BUNDLED) {
        return BundledExecPath();
    } else {
        return p;
    }
}

SettingsWindow::SettingsWindow(QWidget* parent, Qt::WindowFlags f)
    : QDialog(parent, f)
{
    setModal(true);
    setWindowTitle("Preferences");

    auto layout = new QVBoxLayout();
    setLayout(layout);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setMinimumWidth(400);

    // ffmpeg selection
    auto execGroup = new QGroupBox("FFmpeg");
    auto execLayout = new QVBoxLayout();
    execGroup->setLayout(execLayout);
    execGroup->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    layout->addWidget(execGroup);

    auto [container, containerLayout] = CreateContainerWidget<QHBoxLayout>();
    containerLayout->setMargin(0);
    containerLayout->setSpacing(0);
    execLayout->addWidget(container);

    execSelector_ = new QComboBox();
    execSelector_->addItem(BUNDLED);
    execSelector_->insertSeparator(1);
    execSelector_->setMaxCount(6);
    containerLayout->addWidget(execSelector_);

    auto browseBtn = new QPushButton("...");
    browseBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    containerLayout->addWidget(browseBtn);

    execLabel_ = new QLabel();
    execLabel_->setTextFormat(Qt::RichText);
    execLayout->addWidget(execLabel_);

    execPicker_ = new QFileDialog();
    execPicker_->setWindowTitle("Select custom FFmpeg");
    execPicker_->setDirectory(
        QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
    execPicker_->setFileMode(QFileDialog::ExistingFile);

    // Codec selector
    auto codecGroup = new QGroupBox("Preferred Codecs");
    auto codecLayout = new QFormLayout();
    codecGroup->setLayout(codecLayout);
    layout->addWidget(codecGroup);

    auto h264Selector = new QComboBox();
    h264Selector->addItem("libx264");
    codecLayout->addRow("H.264:", h264Selector);

    auto aacSelector = new QComboBox();
    aacSelector->addItem("aac");
    codecLayout->addRow("AAC:", aacSelector);

    auto btnBox =
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    saveBtn_ = btnBox->button(QDialogButtonBox::Ok);
    layout->addWidget(btnBox);

    // Connect signals
    connect(
        execSelector_, &QComboBox::currentTextChanged, this,
        &SettingsWindow::change_exec_);
    connect(
        browseBtn, &QPushButton::clicked, this,
        &SettingsWindow::on_browse_exec_);
    connect(
        btnBox, &QDialogButtonBox::accepted, this,
        &SettingsWindow::close_and_save_);
    connect(
        btnBox, &QDialogButtonBox::rejected, this,
        &SettingsWindow::close_and_reset_);

    // Scan for system ffmpeg and test bundled
    exec_scan_();
    // Load settings
    load_settings_();
}

void SettingsWindow::exec_scan_()
{
    auto exec = QStandardPaths::findExecutable("ffmpeg");
    if (not exec.isEmpty() and execSelector_->findText(exec) == -1) {
        execSelector_->insertItem(2, exec);
    }
}

std::tuple<bool, QString> SettingsWindow::exec_test_(const QString& path)
{
    // Setup encoder
    auto encoder = new QProcess();
    encoder->setProgram(path);

    encoder->setArguments({"-codecs"});
    encoder->start();
    encoder->waitForFinished();
    if (encoder->isOpen()) {
        encoder->kill();
    }

    // TODO: stdout contains available codecs
    auto line = encoder->readAllStandardError();
    qDebug() << "encoder stderr:" << line;
    qDebug() << "encoder stdout:" << encoder->readAllStandardOutput();

    if (encoder->exitCode() != 0) {
        return {false, "Error running program."};
    }

    // Parse stderr for version
    auto idx = VERSION.indexIn(line);
    if (idx < 0) {
        return {false, "Cannot parse executable version."};
    }

    return {true, "Version " + VERSION.cap(1)};
}

void SettingsWindow::exec_msg_success_(const QString& msg)
{
    QString m("<small><font color=\"Green\">✓ ");
    m.append(msg);
    m.append("</font></small>");
    execLabel_->setText(m);
}

void SettingsWindow::exec_msg_error_(const QString& msg)
{
    QString m("<small><font color=\"Red\">✖ ");
    m.append(msg);
    m.append("</font></small>");
    execLabel_->setText(m);
}

void SettingsWindow::on_browse_exec_()
{
    QMessageBox::warning(
        this, tr("ffdropenc"),
        tr("You are about to select a custom FFmpeg executable.  This "
           "application makes no attempt to verify the identity of the "
           "selected file.  For your own safety,  only select a trusted "
           "executable."));
    if (execPicker_->exec()) {
        auto execOpt = execPicker_->selectedFiles()[0];
        if (execSelector_->findText(execOpt) == -1) {
            execSelector_->insertItem(2, execOpt);
        }
        execSelector_->setCurrentText(execOpt);
    } else {
        execSelector_->setCurrentText(current_exec_opt_);
    }
}

void SettingsWindow::change_exec_(const QString& execOpt)
{
    if (execOpt == current_exec_opt_) {
        return;
    }

    auto execPath = ResolveExecPath(execOpt);
    auto [success, msg] = exec_test_(execPath);
    saveBtn_->setEnabled(success);
    if (success) {
        current_exec_opt_ = execOpt;
        exec_msg_success_(msg);
    } else {
        exec_msg_error_(msg);
    }
}

void SettingsWindow::save_settings_()
{
    QSettings settings;
    settings.setValue("ffmpeg/path", current_exec_opt_);
}

void SettingsWindow::load_settings_()
{
    QSettings settings;

    // Load the executable
    auto exec = settings.value("ffmpeg/path", BUNDLED).toString();
    if (fs::exists(ResolveExecPath(exec).toStdString())) {
        if (execSelector_->findText(exec) == -1) {
            execSelector_->insertItem(2, exec);
        }
        execSelector_->setCurrentText(exec);
    } else {
        exec = BUNDLED;
    }
    change_exec_(exec);
}

void SettingsWindow::closeEvent(QCloseEvent* event)
{
    save_settings_();
    event->accept();
}

void SettingsWindow::close_and_save_()
{
    save_settings_();
    emit ExecutableChanged(ResolveExecPath(current_exec_opt_));
    accept();
}

void SettingsWindow::close_and_reset_()
{
    load_settings_();
    reject();
}

QString SettingsWindow::currentExecutablePath() const
{
    return ResolveExecPath(current_exec_opt_);
}