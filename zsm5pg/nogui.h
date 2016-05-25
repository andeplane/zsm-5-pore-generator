#ifndef NOGUI_H
#define NOGUI_H
#include "inifile.h"
#include <QElapsedTimer>
#include <QObject>
#include "statistics/statistic.h"
#include "statistics/concentration.h"

class NoGUI : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Statistic* model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(Statistic* data READ data WRITE setData NOTIFY dataChanged)
    Q_PROPERTY(Statistic* poreSizeDistribution READ poreSizeDistribution WRITE setPoreSizeDistribution NOTIFY poreSizeDistributionChanged)
    Q_PROPERTY(Statistic* cumulativeVolume READ cumulativeVolume WRITE setCumulativeVolume NOTIFY cumulativeVolumeChanged)
    Q_PROPERTY(Statistic* dvlogd READ dvlogd WRITE setDvlogd NOTIFY dvlogdChanged)
    Q_PROPERTY(Concentration* concentration READ concentration WRITE setConcentration NOTIFY concentrationChanged)
private:
    Statistic* m_model = nullptr;
    Statistic* m_data = nullptr;
    Concentration* m_concentration = nullptr;
    Statistic* m_poreSizeDistribution = nullptr;
    Statistic* m_cumulativeVolume = nullptr;
    Statistic* m_dvlogd = nullptr;
    double m_elapsedTime = 0;

public:
    NoGUI();
    int steps = 0; // total timesteps
    int step = 0; // current timestep
    int printEvery = 100;
    class Zsm5geometry *geometry = nullptr;
    class Statistic *statistic = nullptr;
    class MonteCarlo* monteCarlo = nullptr;
    void loadIniFile(IniFile &iniFile);
    Q_INVOKABLE void run();
    Q_INVOKABLE bool tick();
    Statistic* model() const;
    Statistic* data() const;
    Concentration* concentration() const;
    Statistic* poreSizeDistribution() const;
    Statistic* cumulativeVolume() const;
    Statistic* dvlogd() const;

public slots:
    void setModel(Statistic* model);
    void setData(Statistic* data);
    void setConcentration(Concentration* concentration);
    void setPoreSizeDistribution(Statistic* poreSizeDistribution);
    void setCumulativeVolume(Statistic* cumulativeVolume);
    void setDvlogd(Statistic* dvlogd);

signals:
    void modelChanged(Statistic* model);
    void dataChanged(Statistic* data);
    void concentrationChanged(Concentration* concentration);
    void poreSizeDistributionChanged(Statistic* poreSizeDistribution);
    void cumulativeVolumeChanged(Statistic* cumulativeVolume);
    void dvlogdChanged(Statistic* dvlogd);
};

#endif // NOGUI_H
