#include "MainApplication.hpp"
#include "MainLayout.hpp"

int main(int argc, char* argv[])
{
    MainApplication app(argc, argv);
    MainLayout mainWindow;

    // Set the title bar and icon
    mainWindow.setWindowTitle("ffdropenc");
    mainWindow.setMinimumWidth(480);
    // mainWindow.setWindowIcon(QIcon{":/logo-s"});

    mainWindow.show();
    app.exec();

    return EXIT_SUCCESS;
}