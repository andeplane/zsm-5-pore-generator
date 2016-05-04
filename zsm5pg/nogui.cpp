#include "nogui.h"
#include "zsm5geometry.h"
#include "montecarlo.h"
#include "statistics/statistics.h"
#include <QString>
#include <QDebug>
#include <QFileInfo>

NoGUI::NoGUI()
{
    geometry = new Zsm5geometry();
    monteCarlo = new MonteCarlo();
    statistic = new PoreSizeStatistic();
}

void NoGUI::loadIniFile(IniFile &iniFile)
{
    QString statisticType = iniFile.getString("statisticType");
    if(statisticType.compare("poreVolume") == 0) {
        statistic = new PoreVolumeStatistic();
    } else if(statisticType.compare("poreSize") == 0) {
        statistic = new PoreSizeStatistic();
    }

    printEvery = iniFile.getInt("printEvery");

    // Monte carlo properties
    double mcTemperature = iniFile.getDouble("mcTemperature");
    double mcStandardDeviation = iniFile.getDouble("mcStandardDeviation");
    steps = iniFile.getInt("mcSteps");
    monteCarlo->setTemperature(mcTemperature);
    monteCarlo->setStandardDeviation(mcStandardDeviation);
    monteCarlo->setGeometry(geometry);

    int bins = iniFile.getInt("bins");
    float xMin = iniFile.getDouble("xMin");
    float xMax = iniFile.getDouble("xMax");

    if(iniFile.getString("dataType").compare("normalDistribution") == 0) {
        DistributionStatistic *data = new DistributionStatistic();
        data->setNormalDistributionMean(iniFile.getDouble("normalDistributionMean"));
        data->setNormalDistributionStandardDeviation(iniFile.getDouble("normalDistributionStandardDeviation"));
        data->setType(DistributionStatistic::Type::Normal, xMin, xMax, bins);
        monteCarlo->setData(data);
    } else if(iniFile.getString("dataType").compare("exponentialDistribution") == 0) {
        DistributionStatistic *data = new DistributionStatistic();
        data->setExponentialDistributionMean(iniFile.getDouble("exponentialDistributionMean"));
        data->setType(DistributionStatistic::Type::Exponential, xMin, xMax, bins);
        monteCarlo->setData(data);
    } else {
        Statistic *data = new Statistic();
        data->load(iniFile.getString("dataType"));
        monteCarlo->setData(data);
    }

    QFileInfo fileInfo("/projects/poregenerator/currentgeometry.txt");
    bool loadPrevious = iniFile.getBool("loadPrevious") && fileInfo.exists();

    if(loadPrevious) {
        geometry->load(QString("/projects/poregenerator/currentgeometry.txt"));
    } else {
        geometry->setPlanesPerDimension(iniFile.getInt("planesPerDimension"));
        geometry->reset(xMin, xMax);
    }

    statistic->setMin(xMin);
    statistic->setMax(xMax);
    statistic->setBins(bins);
    monteCarlo->setDebug(iniFile.getBool("verbose"));
    monteCarlo->setModel(statistic);
    monteCarlo->setRunning(true);
    statistic->compute(geometry);

//    QVector<float> x = geometry->deltaXVector();
//    QVector<float> y = geometry->deltaYVector();
//    QVector<float> z = geometry->deltaZVector();

//    Statistic *model = monteCarlo->model();
//    Statistic *data = monteCarlo->data();

//    model->compute(geometry);
//    qDebug() << "Chi squared 0: " << model->chiSquared(data);
//    qDebug() << "Chi squared 1: " << model->chiSquared(data);
//    geometry->randomWalkStep(1.0);
//    model->compute(geometry);
//    qDebug() << "Chi squared 2: " << model->chiSquared(data);

//    exit(0);

//    Statistic *model = monteCarlo->model();
//    Statistic *data = monteCarlo->data();
//    qDebug() << "Model: " << model->yValuesRaw();
//    qDebug() << "Data: " << data->yValuesRaw();
//    model->compute(geometry);
//    data->compute(geometry);
//    qDebug() << "Model: " << model->yValuesRaw();
//    qDebug() << "Data: " << data->yValuesRaw();
//    qDebug() << "Chi squared: " << model->chiSquared(data);

//    exit(0);
}

void NoGUI::run() {
    for(int step=0; step<steps; step++) {
        monteCarlo->tick();
        if( (step % printEvery) == 0) {
            qDebug() << "MC step " << step << " of " << steps << ". Current chi squared: " << monteCarlo->chiSquared() << " with acceptance ratio " << monteCarlo->acceptanceRatio();
        }
    }

    geometry->save("/projects/poregenerator/currentgeometry.txt");
    statistic->save("/projects/poregenerator/currenthistogram.txt");

}
