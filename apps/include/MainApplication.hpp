#pragma once

#include <QApplication>

class MainApplication : public QApplication
{
public:
    MainApplication(int& argc, char* argv[]);
    bool event(QEvent* event) override;
};
