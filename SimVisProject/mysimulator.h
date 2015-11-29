#ifndef MYSIMULATOR_H
#define MYSIMULATOR_H
#include <SimVis/Simulator>
#include <SimVis/TriangleCollection>
#include <vector>
#include "linegraph.h"

using std::vector;
#include <QPointF>
#include <QVector>
#include <QVector3D>

struct Settings {
    int planesPerDimension = 32;
    double planeSize = 10;
    int distributionSize = 100;
};

class MyWorker : public SimulatorWorker
{
    Q_OBJECT
private:
    Settings m_settings;
    QVector<double> m_planePositionsX;
    QVector<double> m_planePositionsY;
    QVector<double> m_planePositionsZ;
    vector<vector<vector<double> > > m_volumes;
    QVector<QPointF> m_distribution;
    QVector<QPointF> m_wantedDistribution;
    QVector<SimVis::TriangleCollectionVBOData> m_vertices;
    double volume = 0;
    bool dirty = true;

    // SimulatorWorker interface
    virtual void synchronizeSimulator(Simulator *simulator);
    virtual void synchronizeRenderer(Renderable *renderableObject);
    virtual void work();
    void computeVolume();
    void reset();
    void updateDistribution();
public:
    MyWorker();
};

class MySimulator : public Simulator
{
    Q_OBJECT
    Q_PROPERTY(int planesPerDimension READ planesPerDimension WRITE setPlanesPerDimension NOTIFY planesPerDimensionChanged)
    Q_PROPERTY(double planeSize READ planeSize WRITE setPlaneSize NOTIFY planeSizeChanged)
    Q_PROPERTY(LineGraphDataSource* distribution READ distribution WRITE setDistribution NOTIFY distributionChanged)
    Q_PROPERTY(LineGraphDataSource* wantedDistribution READ wantedDistribution WRITE setWantedDistribution NOTIFY wantedDistributionChanged)
    Q_PROPERTY(double time READ time WRITE setTime NOTIFY timeChanged)
private:
    Settings m_settings;
    bool m_reset = true;
    LineGraphDataSource* m_distribution = nullptr;
    LineGraphDataSource* m_wantedDistribution = nullptr;
    double m_time = 0;

public:
    MySimulator();
    int planesPerDimension() const;
    LineGraphDataSource* distribution() const;
    double planeSize() const;
    double time() const;
    LineGraphDataSource* wantedDistribution() const;

public slots:
    void setPlanesPerDimension(int planesPerDimension);
    void setDistribution(LineGraphDataSource* distribution);
    void setPlaneSize(double planeSize);
    void setTime(double time);
    void setWantedDistribution(LineGraphDataSource* wantedDistribution);

signals:
    void planesPerDimensionChanged(int planesPerDimension);
    void distributionChanged(LineGraphDataSource* distribution);
    void planeSizeChanged(double planeSize);
    void timeChanged(double time);
    void wantedDistributionChanged(LineGraphDataSource* wantedDistribution);

protected:
    virtual SimulatorWorker *createWorker();
    friend class MyWorker;
};

#endif // MYSIMULATOR_H
