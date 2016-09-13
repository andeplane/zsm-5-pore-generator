#ifndef NOGUI_H
#define NOGUI_H
#include "inifile.h"
#include <QElapsedTimer>
#include <QFile>
#include <QObject>
#include "statistics/statistics.h"
#include "montecarlo.h"

class NoGUI : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList statistics READ statistics WRITE setStatistics NOTIFY statisticsChanged)
    Q_PROPERTY(QString filePath READ filePath WRITE setFilePath NOTIFY filePathChanged)
    Q_PROPERTY(MonteCarlo* m_monteCarlo READ m_monteCarlo WRITE setMonteCarlo NOTIFY monteCarloChanged)
    Q_PROPERTY(int timestep READ timestep WRITE setTimestep NOTIFY timestepChanged)
    Q_PROPERTY(int mode READ mode WRITE setMode NOTIFY modeChanged)
    Q_PROPERTY(bool visualize READ visualize WRITE setVisualize NOTIFY visualizeChanged)
private:
    Concentration* m_concentration = nullptr;
    Statistic* m_poreSizeDistribution = nullptr;
    Geometry *m_geometry = nullptr;
    MonteCarlo* m_monteCarlo = nullptr;
    double m_elapsedTime = 0;
    int m_timestep = 0;
    QFile m_log;
    QVariantList m_models;
    QVariantList m_datas;
    QVariantList m_statistics;
    QString m_filePath;
    int m_mode = 0;
    bool m_visualize = false;

public:
    NoGUI();
    ~NoGUI();
    int step = 0; // current timestep
    int printEvery = 100;
    void loadIniFile(IniFile *iniFile);
    Q_INVOKABLE void run();
    Q_INVOKABLE bool tick();
    Concentration* concentration() const;
    Statistic* poreSizeDistribution() const;
    Statistic* currentStatistic() const;
    QVariantList statistics() const;
    MonteCarlo* monteCarlo() const;
    int timestep() const;
    QString filePath() const;
    int mode() const;
    bool visualize() const;

public slots:
    void setConcentration(Concentration* concentration);
    void setPoreSizeDistribution(Statistic* poreSizeDistribution);
    void setCurrentStatistic(Statistic* currentStatistic);
    void setStatistics(QVariantList statistics);
    void setMonteCarlo(MonteCarlo* monteCarlo);
    void setTimestep(int timestep);
    void setFilePath(QString filePath);
    void setMode(int mode);
    void setVisualize(bool visualize);

signals:
    void concentrationChanged(Concentration* concentration);
    void poreSizeDistributionChanged(Statistic* poreSizeDistribution);
    void currentStatisticChanged(Statistic* m_currentStatistic);
    void statisticsChanged(QVariantList statistics);
    void monteCarloChanged(MonteCarlo* m_monteCarlo);
    void timestepChanged(int timestep);
    void filePathChanged(QString filePath);
    void modeChanged(int mode);
    void iniFileChanged(IniFile* iniFile);
    void visualizeChanged(bool visualize);
};

#endif // NOGUI_H
