#ifndef MONTECARLO_H
#define MONTECARLO_H
#include <QObject>
#include "zsm5geometry.h"
#include "statistics/statistics.h"

class MonteCarlo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Zsm5geometry* geometry READ geometry WRITE setGeometry NOTIFY geometryChanged)
    Q_PROPERTY(Statistic* statistic READ statistic WRITE setStatistic NOTIFY statisticChanged)
    Q_PROPERTY(float standardDeviation READ standardDeviation WRITE setStandardDeviation NOTIFY standardDeviationChanged)
    Q_PROPERTY(float temperature READ temperature WRITE setTemperature NOTIFY temperatureChanged)
    Q_PROPERTY(int steps READ steps WRITE setSteps NOTIFY stepsChanged)
    Q_PROPERTY(int accepted READ accepted WRITE setAccepted NOTIFY acceptedChanged)
    Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)
    Zsm5geometry* m_geometry = nullptr;
    Statistic* m_statistic = nullptr;
    float m_standardDeviation = 1.0;
    float m_chiSquared = 0.0;
    double computeChiSquared();
    float m_temperature = 1.0;
    int m_steps = 0;
    int m_accepted = 0;
    bool m_running = false;

public:
    MonteCarlo();
    Zsm5geometry* geometry() const;
    Statistic* statistic() const;
    void tick();
    float standardDeviation() const;
    float temperature() const;
    int steps() const;
    int accepted() const;
    bool running() const;

public slots:
    void setGeometry(Zsm5geometry* geometry);
    void setStatistic(Statistic* statistic);
    void setStandardDeviation(float standardDeviation);
    void setTemperature(float temperature);
    void setSteps(int steps);
    void setAccepted(int accepted);
    void setRunning(bool running);

signals:
    void geometryChanged(Zsm5geometry* geometry);
    void statisticChanged(Statistic* statistic);
    void standardDeviationChanged(float standardDeviation);
    void temperatureChanged(float temperature);
    void stepsChanged(int steps);
    void acceptedChanged(int accepted);
    void runningChanged(bool running);
};

#endif // MONTECARLO_H
