#include "SettingsDialog.hpp"

#include <QDialogButtonBox>
#include <QLabel>
#include <QSpacerItem>
#include <QVBoxLayout>

SettingsDialog::SettingsDialog(QWidget* parent) : QDialog(parent)
{
    QPointer<QVBoxLayout> layout = new QVBoxLayout();
    setLayout(layout);
    setModal(true);
    setWindowTitle("ffdropenc");

    // Preset picker
    layout->addWidget(new QLabel("Select output preset:"));
    presetPicker_ = new QComboBox();
    layout->addWidget(presetPicker_);
    layout->addStretch(1);

    // Input framerate picker
    layout->addWidget(new QLabel("Select input framerate for sequences:"));
    inputFPSPicker_ = new QComboBox();
    inputFPSPicker_->addItem("30");
    layout->addWidget(inputFPSPicker_);
    layout->addStretch(1);

    // Output framerate picker
    // layout->addWidget(new QLabel("Select output framerate for sequences:));
    // outputFPSPicker_ = new QComboBox();
    // outputFPSPicker_->addItem("30");
    // layout->addWidget(outputFPSPicker_);

    // Output directory
    layout->addWidget(new QLabel("Select output directory:"));
    outputDirList_ = new QComboBox();
    outputDirList_->addItem("Same as source");
    layout->addWidget(outputDirList_);
    layout->addStretch(1);

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