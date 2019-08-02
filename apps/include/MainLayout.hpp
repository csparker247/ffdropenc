#pragma once

#include <filesystem>

#include <QLabel>
#include <QMainWindow>
#include <QPointer>
#include <QProgressBar>
#include <QPushButton>
#include <QStringList>
#include <QTextEdit>

class MainLayout : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainLayout(QWidget* parent = nullptr);

    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dragLeaveEvent(QDragLeaveEvent* event) override;
    void dropEvent(QDropEvent* event) override;

    void processFiles(std::vector<std::filesystem::path>& files);

protected:
    QPointer<QLabel> shortLabel;
    QPointer<QProgressBar> progressBar;
    QPointer<QPushButton> cancelBtn;
    QPointer<QTextEdit> details;

    static void load_presets_();
};