#pragma once

#include <filesystem>
#include <vector>

#include <QApplication>

class MainApplication : public QApplication
{
    Q_OBJECT
public:
    MainApplication(int& argc, char* argv[]);

    bool event(QEvent* event) override;

signals:
    void filesDropped(std::vector<std::filesystem::path> p);
};
