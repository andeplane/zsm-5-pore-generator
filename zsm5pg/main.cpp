#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QDebug>
#include <QVector>
#include "mysimulator.h"
#include "statistics/poresizestatistic.h"
#include "zsm5geometry.h"
#include "montecarlo.h"
#define GUI
int main(int argc, char *argv[])
{
#ifdef GUI
    qmlRegisterType<MySimulator>("MySimulator", 1, 0, "MySimulator");
    qmlRegisterType<PoreSizeStatistic>("Zeolite", 1, 0, "PoreSizeStatistic");
    qmlRegisterType<Zsm5geometry>("Zeolite", 1, 0, "Zsm5geometry");
    qmlRegisterType<MonteCarlo>("Zeolite", 1, 0, "MonteCarlo");
    qmlRegisterUncreatableType<Statistic>("Zeolite", 1, 0, "Statistic",
                                          "Cannot create abstract type Statistic. This must be subclassed.");

    QApplication app(argc, argv);

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

