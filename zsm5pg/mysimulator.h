#ifndef MYSIMULATOR_H
#define MYSIMULATOR_H
#include "geometry.h"
#include "planegeometry.h"
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
    Geometry *m_geometry = nullptr;
    Statistic *m_statistic = nullptr;
    MonteCarlo* m_monteCarlo = nullptr;
    unsigned long m_timeElapsed = 0;
    unsigned long m_numberOfTicks = 0;
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
    Q_PROPERTY(Geometry* geometry READ geometry WRITE setGeometry NOTIFY geometryChanged)
    Q_PROPERTY(Statistic* statistic READ statistic WRITE setStatistic NOTIFY statisticChanged)
    Q_PROPERTY(MonteCarlo* monteCarlo READ monteCarlo WRITE setMonteCarlo NOTIFY monteCarloChanged)
    Q_PROPERTY(float tickTime READ tickTime WRITE setTickTime NOTIFY tickTimeChanged)
private:
    bool m_reset = true;
    double m_time = 0;
    Geometry* m_geometry = nullptr;
    Statistic* m_statistic = nullptr;
    MonteCarlo* m_monteCarlo = nullptr;
    float m_tickTime = 0;

public:
    MySimulator();
    ~MySimulator();
    double time() const;
    Geometry* geometry() const;
    Statistic* statistic() const;
    MonteCarlo* monteCarlo() const;
    float tickTime() const;

public slots:
    void setTime(double time);
    void setGeometry(Geometry* geometry);
    void setStatistic(Statistic* statistic);
    void setMonteCarlo(MonteCarlo* monteCarlo);
    void setTickTime(float tickTime);

signals:
    void timeChanged(double time);
    void geometryChanged(Geometry* geometry);
    void statisticChanged(Statistic* statistic);
    void monteCarloChanged(MonteCarlo* monteCarlo);
    void tickTimeChanged(float tickTime);

protected:
    virtual SimulatorWorker *createWorker();
    friend class MyWorker;
};

#endif // MYSIMULATOR_H
