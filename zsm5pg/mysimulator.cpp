#include "mysimulator.h"
#include <QDebug>
#include <cmath>
#include <iostream>
using std::cout; using std::endl;
MySimulator::MySimulator()
{

}

MySimulator::~MySimulator()
{

}

int MySimulator::planesPerDimension() const
{
    return m_settings.planesPerDimension;
}

double MySimulator::planeSize() const
{
    return m_settings.planeSize;
}

double MySimulator::time() const
{
    return m_time;
}

void MySimulator::setPlanesPerDimension(int planesPerDimension)
{
    if (m_settings.planesPerDimension == planesPerDimension)
        return;

    m_settings.planesPerDimension = planesPerDimension;
    emit planesPerDimensionChanged(planesPerDimension);
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

void MyWorker::reset() {
    m_geometry.reset();
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
