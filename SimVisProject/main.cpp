#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QDebug>
#include <QVector>
#include "linegraph.h"
#include "figure.h"
#include "mysimulator.h"
#define GUI
int main(int argc, char *argv[])
{
#ifdef GUI
    qmlRegisterType<Figure>("QMLPlot", 1, 0, "Figure");
    qmlRegisterType<LineGraph>("QMLPlot", 1, 0, "LineGraph");
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

