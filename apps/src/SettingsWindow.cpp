#include "SettingsWindow.hpp"

#include <QApplication>
#include <QDebug>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QProcess>
#include <QStandardPaths>

const QString BUNDLED("Bundled");
QRegExp VERSION{"version\\s(\\d+.\\d+.\\d+)"};

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

    execSelector_ = new QComboBox();
    execSelector_->addItem(BUNDLED);
    execSelector_->insertSeparator(1);
    execSelector_->addItem("Browse...");
    execSelector_->setMaxCount(7);
    execLayout->addWidget(execSelector_);
    execLabel_ = new QLabel();
    execLabel_->setTextFormat(Qt::RichText);
    execLayout->addWidget(execLabel_);

    // Scan for system ffmpeg and test bundled
    // TODO: Test setting saved exec
    exec_scan_();
    exec_test_(BUNDLED);

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
    layout->addWidget(btnBox);
    connect(btnBox, &QDialogButtonBox::accepted, this, &SettingsWindow::accept);
    connect(btnBox, &QDialogButtonBox::rejected, this, &SettingsWindow::reject);
}

void SettingsWindow::exec_scan_()
{
    auto exec = QStandardPaths::findExecutable("ffmpeg");
    if (not exec.isEmpty() and execSelector_->findText(exec) == -1) {
        execSelector_->insertItem(3, exec);
    }
}

void SettingsWindow::exec_test_(const QString& path)
{
    // Setup encoder
    auto encoder = new QProcess();
    if (path == BUNDLED) {
        encoder->setWorkingDirectory(QApplication::applicationDirPath());
        encoder->setProgram("ffmpeg");
    } else {
        encoder->setProgram(path);
    }

    encoder->setArguments({"-codecs"});
    encoder->start();
    encoder->waitForFinished();

    if (encoder->exitCode() != 0) {
        exec_msg_error_("Error running program.");
        // TODO: Block acceptance?
        return;
    }

    // Parse stderr for version
    auto line = encoder->readAllStandardError();
    auto idx = VERSION.indexIn(line);
    if (idx == -1) {
        exec_msg_error_("Cannot parse executable version.");
        // TODO: Block acceptance?
        return;
    } else {
        exec_msg_success_("Version " + VERSION.cap(1));
    }

    // TODO: stdout contains available codecs
    qDebug() << "encoder stdout:" << encoder->readAllStandardOutput();
}

void SettingsWindow::exec_msg_success_(const QString& msg)
{
    QString m("<small><font color=\"Green\">");
    m.append(msg);
    m.append("</font></small>");
    execLabel_->setText(m);
}

void SettingsWindow::exec_msg_error_(const QString& msg)
{
    QString m("<small><font color=\"Red\">");
    m.append(msg);
    m.append("</font></small>");
    execLabel_->setText(m);
}