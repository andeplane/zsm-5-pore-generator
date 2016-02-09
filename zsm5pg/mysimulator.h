#ifndef MYSIMULATOR_H
#define MYSIMULATOR_H
#include "zsm5geometry.h"
#include "distributionanalysis.h"

#include <SimVis/Simulator>
#include <SimVis/TriangleCollection>
#include <vector>
#include <QPointF>
#include <QVector>
#include <QVector3D>
using std::vector;

struct Settings {
    int planesPerDimension = 100;
    double planeSize = 20;
    int distributionSize = 100;
};

class MyWorker : public SimulatorWorker
{
    Q_OBJECT
private:
    Zsm5geometry m_geometry;
    Zsm5geometry m_geometryGradient;
    DistributionAnalysis m_distributionAnalysis;
    Settings m_settings;
    float m_minMse = 1e9;
    float m_initialMse = 1e9;
    float m_mse = 1e9;
    float m_eps = 1e-3;
    QVector<SimVis::TriangleCollectionVBOData> m_vertices;

    // SimulatorWorker interface
    virtual void synchronizeSimulator(Simulator *simulator);
    virtual void synchronizeRenderer(Renderable *renderableObject);
    virtual void work();
    void reset();
public:
    MyWorker();
    ~MyWorker();
    void doWork();
};

class MySimulator : public Simulator
{
    Q_OBJECT
    Q_PROPERTY(int planesPerDimension READ planesPerDimension WRITE setPlanesPerDimension NOTIFY planesPerDimensionChanged)
    Q_PROPERTY(double planeSize READ planeSize WRITE setPlaneSize NOTIFY planeSizeChanged)
    Q_PROPERTY(double time READ time WRITE setTime NOTIFY timeChanged)
private:
    Settings m_settings;
    bool m_reset = true;
    double m_time = 0;

public:
    MySimulator();
    ~MySimulator();
    int planesPerDimension() const;
    double planeSize() const;
    double time() const;

public slots:
    void setPlanesPerDimension(int planesPerDimension);
    void setPlaneSize(double planeSize);
    void setTime(double time);

signals:
    void planesPerDimensionChanged(int planesPerDimension);
    void planeSizeChanged(double planeSize);
    void timeChanged(double time);

protected:
    virtual SimulatorWorker *createWorker();
    friend class MyWorker;
};

#endif // MYSIMULATOR_H
