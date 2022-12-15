#include "MainApplication.hpp"
#include "MainLayout.hpp"

#include <QPointer>

int main(int argc, char* argv[])
{
    QApplication::setOrganizationName("ffdropenc");
    QApplication::setApplicationName("ffdropenc");

    QPointer<MainApplication> app = new MainApplication(argc, argv);
    MainLayout mainWindow;
    mainWindow.show();

    app->exec();

    return EXIT_SUCCESS;
}