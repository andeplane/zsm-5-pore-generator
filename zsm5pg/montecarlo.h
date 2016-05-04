#ifndef MONTECARLO_H
#define MONTECARLO_H
#include <QObject>
#include "zsm5geometry.h"
#include "statistics/statistics.h"

class MonteCarlo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Zsm5geometry* geometry READ geometry WRITE setGeometry NOTIFY geometryChanged)
    Q_PROPERTY(Statistic* model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(Statistic* data READ data WRITE setData NOTIFY dataChanged)
    Q_PROPERTY(float standardDeviation READ standardDeviation WRITE setStandardDeviation NOTIFY standardDeviationChanged)
    Q_PROPERTY(float temperature READ temperature WRITE setTemperature NOTIFY temperatureChanged)
    Q_PROPERTY(int steps READ steps WRITE setSteps NOTIFY stepsChanged)
    Q_PROPERTY(int accepted READ accepted WRITE setAccepted NOTIFY acceptedChanged)
    Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)
    Q_PROPERTY(float chiSquared READ chiSquared WRITE setChiSquared NOTIFY chiSquaredChanged)
    Zsm5geometry* m_geometry = nullptr;
    float m_standardDeviation = 1.0;
    float m_chiSquared = 0.0;
    float m_temperature = 1.0;
    int m_steps = 0;
    int m_accepted = 0;
    bool m_running = false;
    Statistic* m_model = nullptr;
    Statistic* m_data = nullptr;
    bool m_debug = false;

public:
    MonteCarlo();
    Zsm5geometry* geometry() const;
    Statistic* model() const;
    Statistic* data() const;
    void tick();
    float standardDeviation() const;
    float temperature() const;
    int steps() const;
    int accepted() const;
    bool running() const;
    float acceptanceRatio();
    float chiSquared() const;

    void setDebug(bool debug);

public slots:
    void setGeometry(Zsm5geometry* geometry);
    void setStandardDeviation(float standardDeviation);
    void setTemperature(float temperature);
    void setSteps(int steps);
    void setAccepted(int accepted);
    void setRunning(bool running);
    void setModel(Statistic* model);
    void setData(Statistic* data);
    void setChiSquared(float chiSquared);

signals:
    void geometryChanged(Zsm5geometry* geometry);
    void standardDeviationChanged(float standardDeviation);
    void temperatureChanged(float temperature);
    void stepsChanged(int steps);
    void acceptedChanged(int accepted);
    void runningChanged(bool running);
    void modelChanged(Statistic* model);
    void dataChanged(Statistic* data);
    void chiSquaredChanged(float chiSquared);
};

#endif // MONTECARLO_H
