#include "mysimulator.h"
#include <QDebug>
#include <cmath>
#include <iostream>
using std::cout; using std::endl;
MySimulator::MySimulator()
{
    m_distribution = new LineGraphDataSource();
    m_wantedDistribution = new LineGraphDataSource();
}

MySimulator::~MySimulator()
{

}

int MySimulator::planesPerDimension() const
{
    return m_settings.planesPerDimension;
}

LineGraphDataSource *MySimulator::distribution() const
{
    return m_distribution;
}

double MySimulator::planeSize() const
{
    return m_settings.planeSize;
}

double MySimulator::time() const
{
    return m_time;
}

LineGraphDataSource *MySimulator::wantedDistribution() const
{
    return m_wantedDistribution;
}

void MySimulator::setPlanesPerDimension(int planesPerDimension)
{
    if (m_settings.planesPerDimension == planesPerDimension)
        return;

    m_settings.planesPerDimension = planesPerDimension;
    emit planesPerDimensionChanged(planesPerDimension);
}

void MySimulator::setDistribution(LineGraphDataSource *distribution)
{
    if (m_distribution == distribution)
        return;

    m_distribution = distribution;
    emit distributionChanged(distribution);
}

void MySimulator::setPlaneSize(double planeSize)
{
    if (m_settings.planeSize == planeSize)
        return;

    m_settings.planeSize = planeSize;
    emit planeSizeChanged(planeSize);
}

void MySimulator::setTime(double time)
{
    if (m_time == time)
        return;

    m_time = time;
    emit timeChanged(time);
}

void MySimulator::setWantedDistribution(LineGraphDataSource *wantedDistribution)
{
    if (m_wantedDistribution == wantedDistribution)
        return;

    m_wantedDistribution = wantedDistribution;
    emit wantedDistributionChanged(wantedDistribution);
}

SimulatorWorker *MySimulator::createWorker()
{
    return new MyWorker();
}

MyWorker::MyWorker()
{
    reset();
}

MyWorker::~MyWorker()
{

}

void MyWorker::doWork()
{
    work();
}
#include <iostream>
#include <random>

using namespace std;

void MyWorker::reset() {
    m_vertices.clear();

    m_geometry.setPlaneSize(m_settings.planeSize);
    m_geometry.setPlanesPerDimension(m_settings.planesPerDimension);
    m_geometry.reset();
    float lastPlanePosition = 0;
    int seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator (seed);
    // std::exponential_distribution<double> distribution (0.76048);
    float lambda = 0.76048;
    float x0 = 1.5;
    std::exponential_distribution<double> distribution (lambda);

    for(int i=0; i<m_settings.planesPerDimension; i++) {
        float rnd = x0 + distribution(generator);
        m_geometry.planePositionsX().push_back(lastPlanePosition + rnd);
        lastPlanePosition += rnd;
        // cout << rnd << " ";
    }

    lastPlanePosition = 0;
    for(int i=0; i<m_settings.planesPerDimension; i++) {
        float rnd = x0 + distribution(generator);
        m_geometry.planePositionsY().push_back(lastPlanePosition + rnd);
        lastPlanePosition += rnd;
        // cout << rnd << " ";
    }

    lastPlanePosition = 0;
    for(int i=0; i<m_settings.planesPerDimension; i++) {
        float rnd = x0 + distribution(generator);
        m_geometry.planePositionsZ().push_back(lastPlanePosition + rnd);
        lastPlanePosition += rnd;
        // cout << rnd << " ";
    }
    cout << endl;
    m_distributionAnalysis.updateDistribution(m_geometry);
//    m_distributionAnalysis.updateWantedDistribution(m_geometry);
//    m_initialMse = m_distributionAnalysis.meanSquareError(m_geometry);
//    m_minMse = m_initialMse;
//    m_mse = m_initialMse;
}

void MyWorker::synchronizeSimulator(Simulator *simulator)
{
    MySimulator *mySimulator = qobject_cast<MySimulator*>(simulator);
    if(mySimulator) {
        m_settings = mySimulator->m_settings;

        if(mySimulator->m_reset) {
            reset();
            mySimulator->m_reset = false;
        }
        if(mySimulator->distribution()) mySimulator->distribution()->setPoints(m_distributionAnalysis.distribution);
        if(mySimulator->wantedDistribution()) mySimulator->wantedDistribution()->setPoints(m_distributionAnalysis.wantedDistribution);
        mySimulator->setTime(mySimulator->time()+1.0);
    }
}

void MyWorker::synchronizeRenderer(Renderable *renderableObject)
{
    // Synchronize with renderables.
    TriangleCollection* triangleCollection = qobject_cast<TriangleCollection*>(renderableObject);
    if(triangleCollection) {
        triangleCollection->data.clear();
        // Update triangle collection renderable. Similarly if you use other renderables.
        float planeSize = 0.95*std::max(m_geometry.planePositionsX().back(), std::max(m_geometry.planePositionsY().back(), m_geometry.planePositionsZ().back()));

        vector<float> &x = m_geometry.planePositionsX();
        vector<float> &y = m_geometry.planePositionsY();
        vector<float> &z = m_geometry.planePositionsZ();
        for(int planeId=0; planeId<m_settings.planesPerDimension; planeId++) {
            SimVis::TriangleCollectionVBOData p1;
            SimVis::TriangleCollectionVBOData p2;
            SimVis::TriangleCollectionVBOData p3;
            SimVis::TriangleCollectionVBOData p4;
            // First x
            p1.vertex = QVector3D(x[planeId], 0, 0) - 0.5*QVector3D(planeSize, planeSize, planeSize);
            p2.vertex = QVector3D(x[planeId], 0, planeSize) - 0.5*QVector3D(planeSize, planeSize, planeSize);
            p3.vertex = QVector3D(x[planeId], planeSize, planeSize) - 0.5*QVector3D(planeSize, planeSize, planeSize);
            p4.vertex = QVector3D(x[planeId], planeSize, 0) - 0.5*QVector3D(planeSize, planeSize, planeSize);
            QVector3D normal = QVector3D::crossProduct(p1.vertex-p2.vertex, p1.vertex-p3.vertex).normalized();
            QVector3D color(1.0, 1.0, 1.0);
            p1.normal = normal; p2.normal = normal; p3.normal = normal; p4.normal = normal;
            p1.color = color; p2.color = color; p3.color = color; p4.color = color;
            triangleCollection->data.push_back(p1);
            triangleCollection->data.push_back(p2);
            triangleCollection->data.push_back(p3);
            triangleCollection->data.push_back(p1);
            triangleCollection->data.push_back(p3);
            triangleCollection->data.push_back(p4);

            // Then y
            p1.vertex = QVector3D(0, y[planeId], 0) - 0.5*QVector3D(planeSize, planeSize, planeSize);
            p2.vertex = QVector3D(0, y[planeId], planeSize) - 0.5*QVector3D(planeSize, planeSize, planeSize);
            p3.vertex = QVector3D(planeSize, y[planeId], planeSize) - 0.5*QVector3D(planeSize, planeSize, planeSize);
            p4.vertex = QVector3D(planeSize, y[planeId], 0) - 0.5*QVector3D(planeSize, planeSize, planeSize);

            normal = QVector3D::crossProduct(p1.vertex-p2.vertex, p1.vertex-p3.vertex).normalized();
            color = QVector3D(1.0, 1.0, 1.0);
            p1.normal = normal; p2.normal = normal; p3.normal = normal; p4.normal = normal;
            p1.color = color; p2.color = color; p3.color = color; p4.color = color;
            triangleCollection->data.push_back(p1);
            triangleCollection->data.push_back(p2);
            triangleCollection->data.push_back(p3);
            triangleCollection->data.push_back(p1);
            triangleCollection->data.push_back(p3);
            triangleCollection->data.push_back(p4);

            // And z
            p1.vertex = QVector3D(0, 0, z[planeId]) - 0.5*QVector3D(planeSize, planeSize, planeSize);
            p2.vertex = QVector3D(0, planeSize, z[planeId]) - 0.5*QVector3D(planeSize, planeSize, planeSize);;
            p3.vertex = QVector3D(planeSize, planeSize, z[planeId]) - 0.5*QVector3D(planeSize, planeSize, planeSize);;
            p4.vertex = QVector3D(planeSize, 0, z[planeId]) - 0.5*QVector3D(planeSize, planeSize, planeSize);;

            normal = QVector3D::crossProduct(p1.vertex-p2.vertex, p1.vertex-p3.vertex).normalized();
            color = QVector3D(1.0, 1.0, 1.0);
            p1.normal = normal; p2.normal = normal; p3.normal = normal; p4.normal = normal;
            p1.color = color; p2.color = color; p3.color = color; p4.color = color;
            triangleCollection->data.push_back(p1);
            triangleCollection->data.push_back(p2);
            triangleCollection->data.push_back(p3);
            triangleCollection->data.push_back(p1);
            triangleCollection->data.push_back(p3);
            triangleCollection->data.push_back(p4);
        }
        triangleCollection->dirty = true;
    }
}

void MyWorker::work()
{

//    for(int i=0; i<2; i++) {
//        QElapsedTimer t;
//        t.start();
//        float eps = 0.5*m_eps * (m_mse/m_initialMse);
//        eps = 1e-3;
//        m_distributionAnalysis.findGradient(m_geometry, m_geometryGradient);
//        m_geometry.followGradient(m_geometryGradient, eps);
//        m_distributionAnalysis.updateDistribution(m_geometry);
//        m_mse = m_distributionAnalysis.meanSquareError(m_geometry);
//        if(m_mse < m_minMse) {
//            m_minMse = m_mse;
//        }
//        qDebug() << "Elapsed time: " << t.elapsed() << "  eps: " << eps << "   mse: " << m_mse << "   (min mse: " << m_minMse << ")  mean: " << m_distributionAnalysis.currentMean << " (wanted: " << m_distributionAnalysis.wantedMean << ")";
//    }
}
