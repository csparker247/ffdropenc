#include "MainApplication.hpp"

#include <QFileOpenEvent>
#include <QtDebug>

MainApplication::MainApplication(int& argc, char* argv[])
    : QApplication(argc, argv)
{
}

bool MainApplication::event(QEvent* event)
{
    if (event->type() == QEvent::FileOpen) {
        auto openEvent = dynamic_cast<QFileOpenEvent*>(event);
        qDebug() << "Open file" << openEvent->file();
    }

    return QApplication::event(event);
}