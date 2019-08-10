#include "SettingsDialog.hpp"

#include <QDialogButtonBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpacerItem>
#include <QVBoxLayout>

const QString SAME_AS_SOURCE_TEXT("[Same as source]");
const QString SELECT_DIR_TEXT("Browse...");
const QStringList FPS_OPTS{"10", "24", "25", "29.97", "30", "60", "120"};

SettingsDialog::SettingsDialog(QWidget* parent) : QDialog(parent)
{
    QPointer<QVBoxLayout> layout = new QVBoxLayout();
    setLayout(layout);
    setModal(true);
    setSizePolicy({QSizePolicy::Fixed, QSizePolicy::Fixed});
    setWindowTitle("ffdropenc");

    // Preset picker
    auto inputOptsGroup = new QGroupBox();
    inputOptsGroup->setLayout(new QVBoxLayout());
    inputOptsGroup->setTitle("Input Options");
    layout->addWidget(inputOptsGroup);
    layout->addSpacing(5);

    inputOptsGroup->layout()->addWidget(new QLabel("Select output preset:"));
    presetPicker_ = new QComboBox();
    inputOptsGroup->layout()->addWidget(presetPicker_);

    // Framerate Pickers
    auto frameRateGroup = new QGroupBox();
    frameRateGroup->setLayout(new QHBoxLayout());
    frameRateGroup->setTitle("Sequence Options");
    layout->addWidget(frameRateGroup);
    layout->addSpacing(5);

    // Input framerate picker
    auto inputFPSLayout = new QWidget();
    inputFPSLayout->setLayout(new QVBoxLayout());
    inputFPSLayout->layout()->addWidget(new QLabel("Input FPS:"));
    inputFPSPicker_ = new QComboBox();
    inputFPSPicker_->addItems(FPS_OPTS);
    inputFPSPicker_->setCurrentText("30");
    inputFPSLayout->layout()->addWidget(inputFPSPicker_);
    frameRateGroup->layout()->addWidget(inputFPSLayout);

    // Output framerate picker
    auto outputFPSLayout = new QWidget();
    outputFPSLayout->setLayout(new QVBoxLayout());
    outputFPSLayout->layout()->addWidget(new QLabel("Output FPS:"));
    outputFPSPicker_ = new QComboBox();
    outputFPSPicker_->addItem("---");
    outputFPSLayout->layout()->addWidget(outputFPSPicker_);
    frameRateGroup->layout()->addWidget(outputFPSLayout);
    outputFPSLayout->setEnabled(false);

    // Output directory
    auto outputOptsGroup = new QGroupBox();
    outputOptsGroup->setLayout(new QVBoxLayout());
    outputOptsGroup->setTitle("Output Options");
    layout->addWidget(outputOptsGroup);
    layout->addSpacing(5);

    outputOptsGroup->layout()->addWidget(
        new QLabel("Select output directory:"));

    outputDirList_ = new QComboBox();
    outputDirList_->addItem(SAME_AS_SOURCE_TEXT);
    outputDirList_->addItem(SELECT_DIR_TEXT);
    outputDirList_->insertSeparator(1);
    outputDirList_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    outputDirList_->setMaxCount(7);
    outputOptsGroup->layout()->addWidget(outputDirList_);
    currentDir_ = SAME_AS_SOURCE_TEXT;

    outputDirPicker_ = new QFileDialog();
    outputDirPicker_->setFileMode(QFileDialog::Directory);

    connect(
        outputDirList_, &QComboBox::currentTextChanged, this,
        &SettingsDialog::onDirPicked);

    // Standard buttons
    auto buttonBox =
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    layout->addWidget(buttonBox);
}

void SettingsDialog::setPresetList(const QStringList& presetIDs)
{
    presetPicker_->addItems(presetIDs);
}

void SettingsDialog::onDirPicked(const QString& t)
{
    if (t != SELECT_DIR_TEXT) {
        return;
    }

    if (outputDirPicker_->exec()) {
        // Get the selected directory
        auto dir = outputDirPicker_->selectedFiles()[0];

        // If not currently in the list, add it
        QStringList list;
        for (int i = 0; i < outputDirList_->count(); i++) {
            list.append(outputDirList_->itemText(i));
        }
        if (!list.contains(dir)) {
            outputDirList_->insertItem(3, dir);
        }

        // Select the new directory
        outputDirList_->setCurrentText(dir);
        currentDir_ = dir;
    } else {
        outputDirList_->setCurrentText(currentDir_);
    }
}

QString SettingsDialog::getPreset() const
{
    return presetPicker_->currentText();
}

QString SettingsDialog::getInputFPS() const
{
    return inputFPSPicker_->currentText();
}

QString SettingsDialog::getOutputFPS() const
{
    return outputFPSPicker_->currentText();
}

QString SettingsDialog::getOutputDir() const
{
    return outputDirList_->currentText();
}