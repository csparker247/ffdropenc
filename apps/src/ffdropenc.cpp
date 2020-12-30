#include "MainApplication.hpp"
#include "MainLayout.hpp"

#include <QPointer>

int main(int argc, char* argv[])
{
    QApplication::setOrganizationName("ffdropenc");
    QApplication::setApplicationName("ffdropenc");
    QPointer<MainApplication> app = new MainApplication(argc, argv);
    MainLayout mainWindow;

    // Set the title bar and icon
    mainWindow.setWindowTitle("ffdropenc");
    mainWindow.setMinimumWidth(480);
    mainWindow.setMinimumHeight(mainWindow.minimumSizeHint().height());

    mainWindow.show();
    app->exec();

    return EXIT_SUCCESS;
}