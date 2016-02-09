#ifndef MYSIMULATOR_H
#define MYSIMULATOR_H
#include "zsm5geometry.h"
#include "distributionanalysis.h"
#include "statistics/statistic.h"
#include "montecarlo.h"

#include <SimVis/Simulator>
#include <SimVis/TriangleCollection>
#include <vector>
#include <QPointF>
#include <QVector>
#include <QVector3D>
using std::vector;

class MyWorker : public SimulatorWorker
{
    Q_OBJECT
private:
    QVector<SimVis::TriangleCollectionVBOData> m_vertices;
    Zsm5geometry *m_geometry = nullptr;
    Statistic *m_statistic = nullptr;
    MonteCarlo* m_monteCarlo = nullptr;
    // SimulatorWorker interface
    virtual void synchronizeSimulator(Simulator *simulator);
    virtual void synchronizeRenderer(Renderable *renderableObject);
    virtual void work();
public:
    MyWorker();
    ~MyWorker();
    void doWork();
};

class MySimulator : public Simulator
{
    Q_OBJECT
    Q_PROPERTY(double time READ time WRITE setTime NOTIFY timeChanged)
    Q_PROPERTY(Zsm5geometry* geometry READ geometry WRITE setGeometry NOTIFY geometryChanged)
    Q_PROPERTY(Statistic* statistic READ statistic WRITE setStatistic NOTIFY statisticChanged)
    Q_PROPERTY(MonteCarlo* monteCarlo READ monteCarlo WRITE setMonteCarlo NOTIFY monteCarloChanged)

private:
    bool m_reset = true;
    double m_time = 0;
    Zsm5geometry* m_geometry = nullptr;
    Statistic* m_statistic = nullptr;
    MonteCarlo* m_monteCarlo = nullptr;

public:
    MySimulator();
    ~MySimulator();
    double time() const;
    Zsm5geometry* geometry() const;
    Statistic* statistic() const;

    MonteCarlo* monteCarlo() const
    {
        return m_monteCarlo;
    }

public slots:
    void setTime(double time);
    void setGeometry(Zsm5geometry* geometry);
    void setStatistic(Statistic* statistic);

    void setMonteCarlo(MonteCarlo* monteCarlo)
    {
        if (m_monteCarlo == monteCarlo)
            return;

        m_monteCarlo = monteCarlo;
        emit monteCarloChanged(monteCarlo);
    }

signals:
    void timeChanged(double time);
    void geometryChanged(Zsm5geometry* geometry);
    void statisticChanged(Statistic* statistic);

    void monteCarloChanged(MonteCarlo* monteCarlo);

protected:
    virtual SimulatorWorker *createWorker();
    friend class MyWorker;
};

#endif // MYSIMULATOR_H
