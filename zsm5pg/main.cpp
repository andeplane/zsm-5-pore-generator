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
#include <QtCharts>

int main(int argc, char *argv[])
{
//    QString f("/Users/anderhaf/Dropbox/uio/phd/2016/zeolite/adsorption/scripts/Vads.txt");
//    Concentration c(f);
//    exit(1);
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
        qmlRegisterType<Statistic>("Zeolite", 1, 0, "Statistic");

        QApplication app(argc, argv);
        
        QQmlApplicationEngine engine;
        engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
        
        return app.exec();
    } else {
        qmlRegisterType<NoGUI>("Zeolite", 1, 0, "NoGUI");
        qmlRegisterType<Statistic>("Zeolite", 1, 0, "Statistic");

        QString iniFilename;
        if(argc > 2) {
            iniFilename = QString(argv[2]);
        }
        IniFile iniFile(iniFilename);
        NoGUI nogui;
        nogui.loadIniFile(iniFile);

        if(GUI == 2) {
            QApplication app(argc, argv);
            QQmlApplicationEngine engine;
            engine.load(QUrl(QStringLiteral("qrc:/main_simple.qml")));
            for(QObject *root : engine.rootObjects()) {
                root->setProperty("modelStatistic", QVariant::fromValue(nogui.monteCarlo->model()));
                root->setProperty("dataStatistic", QVariant::fromValue(nogui.monteCarlo->data()));
                root->setProperty("poreSizeDistribution", QVariant::fromValue(nogui.poreSizeDistribution()));
                root->setProperty("cumulativeVolume", QVariant::fromValue(nogui.cumulativeVolume()));
                root->setProperty("dvlogd", QVariant::fromValue(nogui.dvlogd()));
                root->setProperty("noGUI", QVariant::fromValue(&nogui));
            }

            return app.exec();
        } else {
            nogui.run();
        }
    }
}
