#include <filesystem>

#include <QApplication>
#include <QMainWindow>

#include "MainLayout.hpp"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QMainWindow mainWindow;

    // Set the title bar and icon
    mainWindow.setCentralWidget(new MainLayout());
    mainWindow.setWindowTitle("ffdropenc");
    mainWindow.setMinimumWidth(480);
    mainWindow.setAcceptDrops(true);
    // mainWindow.setWindowIcon(QIcon{":/logo-s"});

    mainWindow.show();
    app.exec();

    return EXIT_SUCCESS;
}