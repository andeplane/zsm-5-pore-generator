#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QDebug>
#include <QVector>
#include "mysimulator.h"
#define GUI
int main(int argc, char *argv[])
{
#ifdef GUI
    qmlRegisterType<MySimulator>("MySimulator", 1, 0, "MySimulator");
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
#else
    MyWorker worker;
    worker.doWork();
    worker.doWork();
    worker.doWork();
#endif
}

