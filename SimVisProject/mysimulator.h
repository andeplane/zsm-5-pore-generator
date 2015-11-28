#ifndef MYSIMULATOR_H
#define MYSIMULATOR_H
#include <SimVis/Simulator>
#include <SimVis/TriangleCollection>

#include <QVector>
#include <QVector3D>

struct Settings {
    int planesPerDimension = 10;
    double planeSize = 1000;
};

class MyWorker : public SimulatorWorker
{
    Q_OBJECT
private:
    Settings m_settings;
    QVector<double> m_planePositionsX;
    QVector<double> m_planePositionsY;
    QVector<double> m_planePositionsZ;

    QVector<SimVis::TriangleCollectionVBOData> m_vertices;

    // SimulatorWorker interface
    virtual void synchronizeSimulator(Simulator *simulator);
    virtual void synchronizeRenderer(Renderable *renderableObject);
    virtual void work();

    void reset();
public:
    MyWorker();
};

class MySimulator : public Simulator
{
    Q_OBJECT
    Q_PROPERTY(int planesPerDimension READ planesPerDimension WRITE setPlanesPerDimension NOTIFY planesPerDimensionChanged)
private:
    Settings m_settings;
    bool m_reset = true;

public:
    MySimulator();
    int planesPerDimension() const;

public slots:
    void setPlanesPerDimension(int planesPerDimension);

signals:
    void planesPerDimensionChanged(int planesPerDimension);

protected:
    virtual SimulatorWorker *createWorker();
    friend class MyWorker;
};

#endif // MYSIMULATOR_H
