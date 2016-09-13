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
    Q_PROPERTY(MonteCarlo* monteCarlo READ monteCarlo WRITE setMonteCarlo NOTIFY monteCarloChanged)
    Q_PROPERTY(int timestep READ timestep WRITE setTimestep NOTIFY timestepChanged)
    Q_PROPERTY(int mode READ mode WRITE setMode NOTIFY modeChanged)
    Q_PROPERTY(bool visualize READ visualize WRITE setVisualize NOTIFY visualizeChanged)
    Q_PROPERTY(int timesteps READ timesteps WRITE setTimesteps NOTIFY timestepsChanged)
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
    int m_timesteps = 0;

public:
    NoGUI();
    ~NoGUI();
    int printEvery = 100;
    void loadIniFile(IniFile *iniFile);
    Q_INVOKABLE bool tick();
    Q_INVOKABLE void run(int steps);
    QVariantList statistics() const;
    MonteCarlo* monteCarlo() const;
    int timestep() const;
    QString filePath() const;
    int mode() const;
    bool visualize() const;
    int timesteps() const;
    bool isValid();

public slots:
    void setStatistics(QVariantList statistics);
    void setMonteCarlo(MonteCarlo* monteCarlo);
    void setTimestep(int timestep);
    void setFilePath(QString filePath);
    void setMode(int mode);
    void setVisualize(bool visualize);
    void setTimesteps(int timesteps);

signals:
    void statisticsChanged(QVariantList statistics);
    void monteCarloChanged(MonteCarlo* m_monteCarlo);
    void timestepChanged(int timestep);
    void filePathChanged(QString filePath);
    void modeChanged(int mode);
    void iniFileChanged(IniFile* iniFile);
    void visualizeChanged(bool visualize);
    void timestepsChanged(int timesteps);
};

#endif // NOGUI_H
