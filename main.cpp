#include "widget.h"

#include <QApplication>
#include <QSharedMemory>
#include <QTimer>

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
            char* ch = static_cast<char*>(sharedMemory.data());
            *ch = 1;

            Widget w;

            QTimer timer;
            QObject::connect(&timer, &QTimer::timeout, &a, [&sharedMemory, &w]() {
                char *ch = static_cast<char*>(sharedMemory.data());
                // qInfo() << "shared: "<< QString::fromLocal8Bit(ch);
                if (*ch == 0) {
                    // qInfo() << "another process ";
                    w.showAndActivate();
                    *ch = 1;
                }
            });
            timer.start(500);

            w.show();
            return a.exec();
        }else {
            QMessageBox::critical(nullptr, "Error", "cann't create a single instance");
            a.quit();
            return 1;
        }
    }else {
        char* ch = static_cast<char*>(sharedMemory.data());
        // qInfo() << "instance: " << QString::fromLocal8Bit(ch);
        *ch = 0;
        sharedMemory.detach();
        // QMessageBox::critical(nullptr, "warning", "there is one dereshi player is running");
        a.quit();
        return 0;
    }

}
