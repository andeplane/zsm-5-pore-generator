#include "nogui.h"
#include "geometry.h"
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
    setMode(iniFile->getInt("mode"));
    Random::seed(iniFile->getInt("seed"));

    m_log.setFileName(QString("%1/log.txt").arg(m_filePath));
    if (!m_log.open(QIODevice::Append | QIODevice::Text)) {
        qDebug() << "Could not open file " << QString("%1/log.txt").arg(m_filePath);
        exit(1);
    }
    setVisualize(iniFile->getBool("visualize"));
    setTimesteps(iniFile->getInt("mcSteps"));
    setPrintEvery(iniFile->getInt("printEvery"));
    setVerbose(iniFile->getBool("verbose"));
    qDebug() << "NoGUI loaded ini file with ";
    qDebug() << "  Log file: " << m_log.fileName();
    qDebug() << "  Mode: " << m_mode;
    qDebug() << "  Verbose: " << m_verbose;
    qDebug() << "  Seed: " << iniFile->getInt("seed");
    qDebug() << "  Visualize: " << m_visualize;
    qDebug() << "  Timesteps: " << m_timesteps;
    qDebug() << "  Print every: " << m_printEvery;
}

void NoGUI::run(int steps) {
    for(m_timestep=0; m_timestep<steps; m_timestep++) {
        tick();
    }
}

int NoGUI::timesteps() const
{
    return m_timesteps;
}

bool NoGUI::isValid()
{
    for(QVariant &variant : m_statistics) {
        Statistic *statistic = variant.value<Statistic*>();
        if(!statistic->isValid()) return false;
    }

    for(QVariant &variant : m_models) {
        Statistic *statistic = variant.value<Statistic*>();
        if(!statistic->isValid()) return false;
    }

    for(QVariant &variant : m_datas) {
        Statistic *statistic = variant.value<Statistic*>();
        if(!statistic->isValid()) return false;
    }

    if(!m_geometry->isValid()) return false;
    if(!m_monteCarlo->isValid()) return false;

    return true;
}

int NoGUI::printEvery() const
{
    return m_printEvery;
}

Geometry *NoGUI::geometry() const
{
    return m_geometry;
}

bool NoGUI::finished() const
{
    return m_finished;
}

bool NoGUI::verbose() const
{
    return m_verbose;
}

void NoGUI::saveState()
{
    QString fileName = QString("%1/state.ini").arg(m_filePath);
    QFile file(fileName);
    if(!file.open(QFileDevice::WriteOnly | QFileDevice::Text)) {
        qDebug() << "Error, could not not save file " << fileName;
        return;
    }
    if(m_monteCarlo) m_monteCarlo->saveState(file);
    m_geometry->saveState(file);
    for(QVariant &variant : m_statistics) {
        Statistic *statistic = variant.value<Statistic*>();
        statistic->saveState(file);
    }
}

void NoGUI::loadState()
{
    IniFile iniFile;
    iniFile.setFilename( QString("%1/state.ini").arg(m_filePath) );
    if(!iniFile.ready()) return;
    if(m_monteCarlo) m_monteCarlo->loadState(&iniFile);
    m_geometry->loadState(&iniFile);
    for(QVariant &variant : m_statistics) {
        Statistic *statistic = variant.value<Statistic*>();
        statistic->loadState(&iniFile);
    }
}

NoGUI::~NoGUI() {

}

void NoGUI::compute() {
    for(QVariant &variant : m_statistics) {
        Statistic *statistic = variant.value<Statistic*>();
        statistic->compute(m_geometry, m_timestep);
    }

    for(QVariant &variant : m_models) {
        Statistic *statistic = variant.value<Statistic*>();
        statistic->compute(m_geometry, m_timestep);
    }

    for(QVariant &variant : m_datas) {
        Statistic *statistic = variant.value<Statistic*>();
        statistic->compute(m_geometry, m_timestep);
    }

    for(QVariant &variant : m_statistics) {
        Statistic *statistic = variant.value<Statistic*>();
        statistic->compute(m_geometry, m_timestep);
        if(m_visualize) {
            statistic->updateQML();
        }
    }

}

void NoGUI::tick()
{
    if(!isValid()) {
        for(QVariant &variant : m_statistics) {
            Statistic *statistic = variant.value<Statistic*>();
            statistic->compute(m_geometry, m_timestep);
        }

        for(QVariant &variant : m_models) {
            Statistic *statistic = variant.value<Statistic*>();
            statistic->compute(m_geometry, m_timestep);
        }

        for(QVariant &variant : m_datas) {
            Statistic *statistic = variant.value<Statistic*>();
            statistic->compute(m_geometry, m_timestep);
        }
        if(!isValid()) {
            qDebug() << "Error, NoGUI or children not ready...";
            exit(1);
        }
    }
    if(!m_timer.isValid()) m_timer.start();
    if(m_monteCarlo) m_monteCarlo->tick(m_timestep);

    if( m_monteCarlo && (m_timestep % m_printEvery) == 0) {
        double timeLeft = m_timer.elapsed() / ( double(m_timestep+1)) * (m_timesteps-m_timestep) / 1000.; // seconds
        QTextStream logStream(&m_log);
        qDebug() << "MC step " << m_timestep << "/" << m_timesteps << ". χ^2: " << QString::number( m_monteCarlo->chiSquared(), 'f', 10 ) << ", T=" << m_monteCarlo->temperature() << " with acceptance ratio " << m_monteCarlo->acceptanceRatio() << " (" << m_monteCarlo->accepted() << " / " << m_monteCarlo->steps() << ") and random walk fraction " << m_geometry->randomWalkFraction() << ". Estimated time left: " << timeLeft << " seconds.";
        logStream << "MC step " << m_timestep << "/" << m_timesteps << ". χ^2: " << QString::number( m_monteCarlo->chiSquared(), 'f', 10 ) << ", T=" << m_monteCarlo->temperature() << " with acceptance ratio " << m_monteCarlo->acceptanceRatio() << " (" << m_monteCarlo->accepted() << " / " << m_monteCarlo->steps() << ") and random walk fraction " << m_geometry->randomWalkFraction() << ". Estimated time left: " << timeLeft << " seconds.\n";
        if(m_visualize) {
            for(QVariant &variant : m_statistics) {
                Statistic *statistic = variant.value<Statistic*>();
                statistic->compute(m_geometry, m_timestep);
                if(m_visualize) {
                    statistic->updateQML();
                }
            }
        }
    }

    m_timestep++;

    if(m_timestep >= m_timesteps) {
        setFinished(true);
    }
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

void NoGUI::setTimesteps(int timesteps)
{
    if (m_timesteps == timesteps)
        return;

    m_timesteps = timesteps;
    emit timestepsChanged(timesteps);
}

void NoGUI::setPrintEvery(int printEvery)
{
    if (m_printEvery == printEvery)
        return;

    m_printEvery = printEvery;
    emit printEveryChanged(printEvery);
}

void NoGUI::setGeometry(Geometry *geometry)
{
    if (m_geometry == geometry)
        return;

    m_geometry = geometry;
    emit geometryChanged(geometry);
}

void NoGUI::setFinished(bool finished)
{
    if (m_finished == finished)
        return;

    m_finished = finished;
    emit finishedChanged(finished);
}

void NoGUI::setVerbose(bool verbose)
{
    if (m_verbose == verbose)
        return;

    m_verbose = verbose;
    emit verboseChanged(verbose);
}

