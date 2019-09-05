#include "MainApplication.hpp"
#include "MainLayout.hpp"

#include <QScopedPointer>

int main(int argc, char* argv[])
{
    QPointer<MainApplication> app = new MainApplication(argc, argv);
    MainLayout mainWindow;

    // Set the title bar and icon
    mainWindow.setWindowTitle("ffdropenc");
    mainWindow.setMinimumWidth(480);
    mainWindow.setMinimumHeight(mainWindow.minimumSizeHint().height());
    // mainWindow.setWindowIcon(QIcon{":/logo-s"});

    mainWindow.show();
    app->exec();

    return EXIT_SUCCESS;
}