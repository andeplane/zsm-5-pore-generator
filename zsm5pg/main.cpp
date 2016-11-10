#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QDebug>
#include <QVector>
#include "statistics/statistics.h"
#include "geometry.h"
#include "montecarlo.h"
#include "mcobject.h"
#include "inifile.h"
#include "nogui.h"
#include <QtCharts>
#include "random.h"

int main(int argc, char *argv[])
{
    qmlRegisterType<NoGUI>("Zeolite", 1, 0, "NoGUI");
    qmlRegisterType<MCObject>("Zeolite", 1, 0, "MCObject");
    qmlRegisterType<Statistic>("Zeolite", 1, 0, "Statistic");
    qmlRegisterType<Concentration>("Zeolite", 1, 0, "Concentration");
    qmlRegisterType<PoreSizeStatistic>("Zeolite", 1, 0, "PoreSizeStatistic");
    qmlRegisterType<PoreVolumeStatistic>("Zeolite", 1, 0, "PoreVolumeStatistic");
    qmlRegisterType<DVDLogd>("Zeolite", 1, 0, "DVDLogd");
    qmlRegisterType<LengthRatio>("Zeolite", 1, 0, "LengthRatio");
    qmlRegisterType<CumulativeVolume>("Zeolite", 1, 0, "CumulativeVolume");
    qmlRegisterType<Geometry>("Zeolite", 1, 0, "Geometry");
    qmlRegisterType<MonteCarlo>("Zeolite", 1, 0, "MonteCarlo");
    qmlRegisterType<IniFile>("Zeolite", 1, 0, "IniFile");

    QString iniFilename;
    if(argc > 1) {
        iniFilename = QString(argv[1]);
    } else {
        qDebug() << "Error, did not provide ini file.";
        return 1;
    }

    bool sliders = false;
    if(argc>2) {
        sliders = atoi(argv[2]);
    }

    QApplication app(argc, argv);
    QQmlApplicationEngine engine;
    if(sliders) {
        engine.load(QUrl(QStringLiteral("qrc:/main_sliders.qml")));
    } else {
        engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    }
    for(QObject *root : engine.rootObjects()) {
        root->setProperty("iniFilename", QVariant::fromValue(iniFilename));
    }

    return app.exec();
}
