#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QDebug>
#include <QVector>
#include "mysimulator.h"
#include "statistics/statistics.h"
#include "zsm5geometry.h"
#include "montecarlo.h"
#include "inifile.h"
#include "nogui.h"

int main(int argc, char *argv[])
{

    int GUI = true;

    if(argc > 1) {
        GUI = atoi(argv[1]);
    }

    if(GUI == 1) {
        qmlRegisterType<MySimulator>("MySimulator", 1, 0, "MySimulator");
        qmlRegisterType<PoreSizeStatistic>("Zeolite", 1, 0, "PoreSizeStatistic");
        qmlRegisterType<PoreVolumeStatistic>("Zeolite", 1, 0, "PoreVolumeStatistic");
        qmlRegisterType<Zsm5geometry>("Zeolite", 1, 0, "Zsm5geometry");
        qmlRegisterType<MonteCarlo>("Zeolite", 1, 0, "MonteCarlo");
        qmlRegisterUncreatableType<Statistic>("Zeolite", 1, 0, "Statistic",
                                              "Cannot create abstract type Statistic. This must be subclassed.");
        
        QApplication app(argc, argv);
        
        QQmlApplicationEngine engine;
        engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
        
        return app.exec();
    } else {
        QString iniFilename;
        if(argc > 2) {
            iniFilename = QString(argv[2]);
        }
        IniFile iniFile(iniFilename);
        NoGUI nogui;
        nogui.loadIniFile(iniFile);

        if(GUI == 2) {

        } else {
            nogui.run();
        }
    }
}
