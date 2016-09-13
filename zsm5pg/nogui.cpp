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

}

void NoGUI::loadIniFile(IniFile *iniFile)
{
    QFileInfo fileInfo(iniFile->filename());
    setFilePath(fileInfo.absolutePath());
    setMode(iniFile.getInt("mode"));
    Random::seed(iniFile->getInt("seed"));

    m_log.setFileName(QString("%1/log.txt").arg(m_filePath));
    if (!m_log.open(QIODevice::Append | QIODevice::Text)) {
        qDebug() << "Could not open file " << QString("%1/log.txt").arg(m_filePath);
        exit(1);
    }
    setVisualize(iniFile.getInt("visualize"));
}

void NoGUI::run(int steps) {
    for(step=0; step<steps; step++) {
        tick();
    }
}

NoGUI::~NoGUI() {

}

bool NoGUI::tick()
{
    QElapsedTimer timer;
    timer.start();
    m_monteCarlo->tick();
    m_elapsedTime += timer.elapsed();

    step++;
    if( (step % printEvery) == 0) {
        double timeLeft = m_elapsedTime / (step+1) * (steps-step) / 1000.; // seconds
        QTextStream logStream(&m_log);
        qDebug() << "MC step " << step << "/" << steps << ". χ^2: " << m_monteCarlo->chiSquared() << " with acceptance ratio " << m_monteCarlo->acceptanceRatio() << " and random walk fraction " << geometry->randomWalkFraction() << ". Estimated time left: " << timeLeft << " seconds.";
        logStream << "MC step " << step << "/" << steps << ". χ^2: " << m_monteCarlo->chiSquared() << " with acceptance ratio " << m_monteCarlo->acceptanceRatio() << " and random walk fraction " << geometry->randomWalkFraction() << ". Estimated time left: " << timeLeft << " seconds.\n";
        if(m_visualize) {
            for(QVariant &variant : m_statistics) {
                Statistic *statistic = variant.value<Statistic*>();
                statistic->compute(m_geometry);
                statistic->updateQML();
            }
        }
    }

    return false;
}

Concentration *NoGUI::concentration() const
{
    return m_concentration;
}

Statistic *NoGUI::poreSizeDistribution() const
{
    return m_poreSizeDistribution;
}

Statistic *NoGUI::currentStatistic() const
{
    return m_currentStatistic;
}

QVariantList NoGUI::statistics() const
{
    return m_statistics;
}

MonteCarlo *NoGUI::monteCarlo() const
{
    return m_monteCarlo;
}

int NoGUI::timestep() const
{
    return m_timestep;
}

QString NoGUI::filePath() const
{
    return m_filePath;
}

int NoGUI::mode() const
{
    return m_mode;
}

bool NoGUI::visualize() const
{
    return m_visualize;
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

void NoGUI::setCurrentStatistic(Statistic *currentStatistic)
{
    if (m_currentStatistic == currentStatistic)
        return;

    m_currentStatistic = currentStatistic;
    emit currentStatisticChanged(currentStatistic);
}

void NoGUI::setStatistics(QVariantList statistics)
{
    if (m_statistics == statistics)
        return;

    m_statistics = statistics;
    emit statisticsChanged(statistics);
}

void NoGUI::setMonteCarlo(MonteCarlo *monteCarlo)
{
    if (m_monteCarlo == monteCarlo)
        return;

    m_monteCarlo = monteCarlo;
    emit monteCarloChanged(monteCarlo);
}

void NoGUI::setTimestep(int timestep)
{
    if (m_timestep == timestep)
        return;

    m_timestep = timestep;
    emit timestepChanged(timestep);
}

void NoGUI::setFilePath(QString filePath)
{
    if (m_filePath == filePath)
        return;

    m_filePath = filePath;
    emit filePathChanged(filePath);
}

void NoGUI::setMode(int mode)
{
    if (m_mode == mode)
        return;

    m_mode = mode;
    emit modeChanged(mode);
}

void NoGUI::setVisualize(bool visualize)
{
    if (m_visualize == visualize)
        return;

    m_visualize = visualize;
    emit visualizeChanged(visualize);
}

