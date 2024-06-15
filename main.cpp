#include "widget.h"

#include <QApplication>
#include <QSharedMemory>
#include <QTimer>
#include <QtDBus>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSharedMemory sharedMemory("dereshi-player", &a);

    if(!sharedMemory.attach()){
        if(sharedMemory.create(1)){
            QObject::connect(&a, &QApplication::aboutToQuit, &a, [&sharedMemory]() {
                sharedMemory.detach();
                // qInfo() << "detach when close ";
            });
            // char* ch = static_cast<char*>(sharedMemory.data());
            // sharedMemory.lock();
            // *ch = 1;
            // sharedMemory.unlock();
            Widget w;

            // QTimer timer;
            // QObject::connect(&timer, &QTimer::timeout, &a, [&sharedMemory, &w]() {
            //     char *ch = static_cast<char*>(sharedMemory.data());
            //     qInfo() << "shared: "<< QString::fromUtf8(ch);
            //     if (*ch == 0) {
            //         // qInfo() << "another process ";
            //         w.showAndActivate();
            //         sharedMemory.lock();
            //         *ch = 1;
            //         sharedMemory.unlock();
            //     }
            // });
            // timer.start(500);


            QDBusConnection::sessionBus().registerObject("/com/player/dereshi", &w, QDBusConnection::ExportAllSlots);

            QDBusConnection::sessionBus().registerService("com.player.dereshi");

            w.show();
            return a.exec();
        }else {
            QMessageBox::critical(nullptr, "Error", "cann't create a single instance");
            a.quit();
            return 1;
        }
    }else {
        // char* ch = static_cast<char*>(sharedMemory.data());
        // qInfo() << "instance: " << QString::fromUtf8(ch);
        // sharedMemory.lock();
        // *ch = 0;
        // sharedMemory.unlock();

        QDBusInterface interface("com.player.dereshi", "/com/player/dereshi", "", QDBusConnection::sessionBus());
        interface.call("showAndActivate");
        sharedMemory.detach();
        // QMessageBox::critical(nullptr, "warning", "there is one dereshi player is running");
        a.quit();
        return 0;
    }

}
