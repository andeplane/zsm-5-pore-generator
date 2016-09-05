#include "nogui.h"
#include "zsm5geometry.h"
#include "montecarlo.h"
#include "statistics/statistics.h"
#include <QString>
#include <QDebug>
#include <QFileInfo>
#include "random.h"
NoGUI::NoGUI()
{
    geometry = new Zsm5geometry();
    monteCarlo = new MonteCarlo();
    statistic = new PoreSizeStatistic();
    m_poreSizeDistribution = new PoreSizeStatistic();
    m_poreSizeDistribution->setMin(0);
    m_poreSizeDistribution->setMax(20);
    m_poreSizeDistribution->setBins(100);

    m_cumulativeVolume = new CumulativeVolume();
    m_cumulativeVolume->setMin(0);
    m_cumulativeVolume->setMax(20);
    m_cumulativeVolume->setBins(100);

    m_dvlogd = new DVDLogd();
    m_dvlogd->setMin(0);
    m_dvlogd->setMax(20);
    m_dvlogd->setBins(100);

    m_lengthRatio = new LengthRatio();
    m_lengthRatio->setMin(0);
    m_lengthRatio->setMax(1);
    m_lengthRatio->setBins(50);
}

void NoGUI::loadIniFile(IniFile &iniFile)
{
    QFileInfo fileInfo(iniFile.filename());
    m_filepath = fileInfo.absolutePath();
    Random::seed(iniFile.getInt("seed"));

    m_log.setFileName(QString("%1/log.txt").arg(m_filepath));
    if (!m_log.open(QIODevice::Append | QIODevice::Text)) {
        qDebug() << "Could not open file " << QString("%1/log.txt").arg(m_filepath);
        exit(1);
    }

    QString MDInput = iniFile.getString("MDInput");
    QString statisticType = iniFile.getString("statisticType");
    if(statisticType.compare("poreVolume") == 0) {
        statistic = new PoreVolumeStatistic();
    } else if(statisticType.compare("poreSize") == 0) {
        statistic = new PoreSizeStatistic();
    } else if(statisticType.compare("concentration") == 0) {
        statistic = new Concentration(MDInput);
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
        data->setName("Data statistic");
        data->load(iniFile.getString("dataType"));
        monteCarlo->setData(data);
    }

    QFileInfo fileInfo2(QString("%1/geometry.txt").arg(m_filepath));
    int planesPerDimension = iniFile.getInt("planesPerDimension");

    bool loadPrevious = iniFile.getBool("loadPrevious") && fileInfo2.exists();
    if(loadPrevious) {
        geometry->load(QString("%1/geometry.txt").arg(m_filepath));
    } else {
        geometry->setPlanesPerDimension(planesPerDimension);
        geometry->reset(xMin, xMax);
    }

    if(geometry->planesPerDimension() != planesPerDimension) {
        qDebug() << "Resizing from " << geometry->planesPerDimension() << " planes per dimension to " << planesPerDimension;
        geometry->resize(planesPerDimension);
    }

    float randomWalkFraction = iniFile.getDouble("randomWalkFraction");
    geometry->setRandomWalkFraction(randomWalkFraction);

    statistic->setMin(xMin);
    statistic->setMax(xMin + 3*delta);
    statistic->setBins(bins);
    m_poreSizeDistribution->setBins(bins);
    m_cumulativeVolume->setBins(bins);
    m_dvlogd->setBins(bins);
    m_lengthRatio->setBins(bins);
    monteCarlo->setFilename(QString("%1/%2").arg(m_filepath).arg(iniFile.getString("mcFilename")));
    monteCarlo->setDebug(iniFile.getBool("verbose"));
    monteCarlo->setModel(statistic);
    monteCarlo->setRunning(true);
    statistic->compute(geometry);

    m_data = monteCarlo->data();
    m_model = monteCarlo->model();
    monteCarlo->model()->updateQML();
    monteCarlo->data()->updateQML();

    m_concentration = new Concentration(MDInput);
}

void NoGUI::run() {
    monteCarlo->data()->createLineSeries();
    for(step=0; step<steps; step++) {
        QElapsedTimer timer;
        timer.start();
        monteCarlo->tick();
        m_elapsedTime += timer.elapsed();
        if( (step % printEvery) == 0) {
            double timeLeft = m_elapsedTime / (step+1) * (steps-step) / 1000.; // seconds
            QTextStream logStream(&m_log);
            qDebug() << "MC step " << step << "/" << steps << ". χ^2: " << monteCarlo->chiSquared() << " with acceptance ratio " << monteCarlo->acceptanceRatio() << " and random walk fraction " << geometry->randomWalkFraction() << ". Estimated time left: " << timeLeft << " seconds.";
            logStream << "MC step " << step << "/" << steps << ". χ^2: " << monteCarlo->chiSquared() << " with acceptance ratio " << monteCarlo->acceptanceRatio() << " and random walk fraction " << geometry->randomWalkFraction() << ". Estimated time left: " << timeLeft << " seconds.\n";
        }
    }

    geometry->save("/projects/poregenerator/currentgeometry.txt");
    statistic->save("/projects/poregenerator/currenthistogram.txt");
}

NoGUI::~NoGUI() {
    m_poreSizeDistribution->compute(geometry);
    m_cumulativeVolume->compute(geometry);
    m_dvlogd->compute(geometry);
    m_lengthRatio->compute(geometry);
    m_concentration->compute(geometry);
    m_model->compute(geometry);

    if(m_model) m_model->save(QString("%1/model.txt").arg(m_filepath));
    if(m_data) m_data->save(QString("%1/data.txt").arg(m_filepath));
    if(m_poreSizeDistribution) m_poreSizeDistribution->save(QString("%1/poreSizeDistribution.txt").arg(m_filepath));
    if(m_concentration) m_concentration->save(QString("%1/concentration.txt").arg(m_filepath));
    if(m_cumulativeVolume) m_cumulativeVolume->save(QString("%1/cumulativeVolume.txt").arg(m_filepath));
    if(m_dvlogd) m_dvlogd->save(QString("%1/dvlogd.txt").arg(m_filepath));
    if(m_lengthRatio) m_lengthRatio->save(QString("%1/lengthRatio.txt").arg(m_filepath));
    if(geometry) geometry->save(QString("%1/geometry.txt").arg(m_filepath));
    QFile chiSquareFile(QString("%1/ChiSquare=%2").arg(m_filepath).arg(monteCarlo->chiSquared()));
    chiSquareFile.open(QIODevice::WriteOnly | QIODevice::Text);
    chiSquareFile.close();

    chiSquareFile.setFileName(QString("%1/ChiSquare.txt").arg(m_filepath));
    if(!chiSquareFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Error, could not open chi square file: " << chiSquareFile.fileName();
        return;
    }
    QTextStream in(&chiSquareFile);
    in << monteCarlo->chiSquared();
    chiSquareFile.close();
}

bool NoGUI::tick()
{
    if(step >= steps) {
        geometry->save("/projects/poregenerator/currentgeometry.txt");
        statistic->save("/projects/poregenerator/currenthistogram.txt");

        return true;
    }

    QElapsedTimer timer;
    timer.start();
    monteCarlo->tick();
    m_elapsedTime += timer.elapsed();

    step++;
    if( (step % printEvery) == 0) {
        double timeLeft = m_elapsedTime / (step+1) * (steps-step) / 1000.; // seconds
        QTextStream logStream(&m_log);
        qDebug() << "MC step " << step << "/" << steps << ". χ^2: " << monteCarlo->chiSquared() << " with acceptance ratio " << monteCarlo->acceptanceRatio() << " and random walk fraction " << geometry->randomWalkFraction() << ". Estimated time left: " << timeLeft << " seconds.";
        logStream << "MC step " << step << "/" << steps << ". χ^2: " << monteCarlo->chiSquared() << " with acceptance ratio " << monteCarlo->acceptanceRatio() << " and random walk fraction " << geometry->randomWalkFraction() << ". Estimated time left: " << timeLeft << " seconds.\n";
        m_poreSizeDistribution->compute(geometry);
        m_poreSizeDistribution->updateQML();

        m_cumulativeVolume->compute(geometry);
        m_cumulativeVolume->updateQML();

        m_dvlogd->compute(geometry);
        m_dvlogd->updateQML();

        m_lengthRatio->compute(geometry);
        m_lengthRatio->updateQML();

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

Statistic *NoGUI::cumulativeVolume() const
{
    return m_cumulativeVolume;
}

Statistic *NoGUI::dvlogd() const
{
    return m_dvlogd;
}

Statistic *NoGUI::lengthRatio() const
{
    return m_lengthRatio;
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

void NoGUI::setCumulativeVolume(Statistic *cumulativeVolume)
{
    if (m_cumulativeVolume == cumulativeVolume)
        return;

    m_cumulativeVolume = cumulativeVolume;
    emit cumulativeVolumeChanged(cumulativeVolume);
}

void NoGUI::setDvlogd(Statistic *dvlogd)
{
    if (m_dvlogd == dvlogd)
        return;

    m_dvlogd = dvlogd;
    emit dvlogdChanged(dvlogd);
}

void NoGUI::setLengthRatio(Statistic *lengthRatio)
{
    if (m_lengthRatio == lengthRatio)
        return;

    m_lengthRatio = lengthRatio;
    emit lengthRatioChanged(lengthRatio);
}

