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
    m_poreSizeDistribution = new PoreSizeStatistic();
    m_poreSizeDistribution->setMin(0);
    m_poreSizeDistribution->setMax(20);
    QString f("/Users/anderhaf/Dropbox/uio/phd/2016/zeolite/adsorption/scripts/Vads.txt");
    m_concentration = new Concentration(f);
}

void NoGUI::loadIniFile(IniFile &iniFile)
{
    QString statisticType = iniFile.getString("statisticType");
    if(statisticType.compare("poreVolume") == 0) {
        statistic = new PoreVolumeStatistic();
    } else if(statisticType.compare("poreSize") == 0) {
        statistic = new PoreSizeStatistic();
    } else if(statisticType.compare("concentration") == 0) {
        statistic = new Concentration("/Users/anderhaf/Dropbox/uio/phd/2016/zeolite/adsorption/scripts/Vads.txt");
    } else {
        qDebug() << "Error, could not find statistic type " << statisticType;
        exit(1);
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
    float delta = xMax - xMin;

    if(iniFile.getString("dataType").compare("normalDistribution") == 0) {
        DistributionStatistic *data = new DistributionStatistic();
        data->setNormalDistributionMean(iniFile.getDouble("normalDistributionMean"));
        data->setNormalDistributionStandardDeviation(iniFile.getDouble("normalDistributionStandardDeviation"));
        data->setType(DistributionStatistic::Type::Normal, xMin, xMin+3*delta, bins);
        monteCarlo->setData(data);
    } else if(iniFile.getString("dataType").compare("exponentialDistribution") == 0) {
        DistributionStatistic *data = new DistributionStatistic();
        data->setExponentialDistributionMean(iniFile.getDouble("exponentialDistributionMean"));
        data->setType(DistributionStatistic::Type::Exponential, xMin, xMin+3*delta, bins);
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
    statistic->setMax(xMin + 3*delta);
    statistic->setBins(bins);
    monteCarlo->setDebug(iniFile.getBool("verbose"));
    monteCarlo->setModel(statistic);
    monteCarlo->setRunning(true);
    statistic->compute(geometry);

    monteCarlo->model()->updateQML();
    monteCarlo->data()->updateQML();
    qDebug() << "I'm done with this.";
}

void NoGUI::run() {
    monteCarlo->data()->createLineSeries();
    for(step=0; step<steps; step++) {
        qDebug() << "yeah?";
        monteCarlo->tick();
        if( (step % printEvery) == 0) {
            qDebug() << "MC step " << step << " of " << steps << ". Current chi squared: " << monteCarlo->chiSquared() << " with acceptance ratio " << monteCarlo->acceptanceRatio();
        }
    }

    geometry->save("/projects/poregenerator/currentgeometry.txt");
    statistic->save("/projects/poregenerator/currenthistogram.txt");
}

bool NoGUI::tick()
{
    if(step >= steps) {
        geometry->save("/projects/poregenerator/currentgeometry.txt");
        statistic->save("/projects/poregenerator/currenthistogram.txt");

        return true;
    }

    monteCarlo->tick();
    step++;
    if( (step % printEvery) == 0) {
        qDebug() << "MC step " << step << " of " << steps << ". Current chi squared: " << monteCarlo->chiSquared() << " with acceptance ratio " << monteCarlo->acceptanceRatio();
        m_poreSizeDistribution->compute(geometry);
        m_poreSizeDistribution->updateQML();
        monteCarlo->model()->updateQML();
        m_concentration->compute(geometry);
    }

    return false;
}

Statistic *NoGUI::model() const
{
    return m_model;
}

Statistic *NoGUI::data() const
{
    return m_data;
}

Concentration *NoGUI::concentration() const
{
    return m_concentration;
}

Statistic *NoGUI::poreSizeDistribution() const
{
    return m_poreSizeDistribution;
}

void NoGUI::setModel(Statistic *model)
{
    if (m_model == model)
        return;

    m_model = model;
    emit modelChanged(model);
}

void NoGUI::setData(Statistic *data)
{
    if (m_data == data)
        return;

    m_data = data;
    emit dataChanged(data);
}

void NoGUI::setConcentration(Concentration *concentration)
{
    if (m_concentration == concentration)
        return;

    m_concentration = concentration;
    emit concentrationChanged(concentration);
}

void NoGUI::setPoreSizeDistribution(Statistic *poreSizeDistribution)
{
    if (m_poreSizeDistribution == poreSizeDistribution)
        return;

    m_poreSizeDistribution = poreSizeDistribution;
    emit poreSizeDistributionChanged(poreSizeDistribution);
}
