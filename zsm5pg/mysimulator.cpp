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

double MySimulator::time() const
{
    return m_time;
}

Zsm5geometry *MySimulator::geometry() const
{
    return m_geometry;
}

Statistic *MySimulator::statistic() const
{
    return m_statistic;
}

void MySimulator::setTime(double time)
{
    if (m_time == time)
        return;

    m_time = time;
    emit timeChanged(time);
}

void MySimulator::setGeometry(Zsm5geometry *geometry)
{
    if (m_geometry == geometry)
        return;

    m_geometry = geometry;
    emit geometryChanged(geometry);
}

void MySimulator::setStatistic(Statistic *statistic)
{
    if (m_statistic == statistic)
        return;

    m_statistic = statistic;
    emit statisticChanged(statistic);
}

SimulatorWorker *MySimulator::createWorker()
{
    return new MyWorker();
}

MyWorker::MyWorker()
{

}

MyWorker::~MyWorker()
{

}

void MyWorker::doWork()
{
    work();
}

void MyWorker::synchronizeSimulator(Simulator *simulator)
{
    MySimulator *mySimulator = qobject_cast<MySimulator*>(simulator);
    if(mySimulator) {
        m_geometry = mySimulator->geometry();
        m_statistic = mySimulator->statistic();
        if(m_geometry && mySimulator->m_reset) {
            m_geometry->reset();
            mySimulator->m_reset = false;
        }
        mySimulator->setTime(mySimulator->time()+1.0);
    }
}

void MyWorker::synchronizeRenderer(Renderable *renderableObject)
{
    // Synchronize with renderables.
    TriangleCollection* triangleCollection = qobject_cast<TriangleCollection*>(renderableObject);
    if(triangleCollection && m_geometry) {
        triangleCollection->data.clear();
        // Update triangle collection renderable. Similarly if you use other renderables.
        vector<float> &x = m_geometry->deltaXVector();
        vector<float> &y = m_geometry->deltaYVector();
        vector<float> &z = m_geometry->deltaZVector();
        float maxPlaneCoordinateX = 0;
        float maxPlaneCoordinateY = 0;
        float maxPlaneCoordinateZ = 0;
        for(int planeId=0; planeId<m_geometry->planesPerDimension(); planeId++) {
            maxPlaneCoordinateX += x[planeId];
            maxPlaneCoordinateY += y[planeId];
            maxPlaneCoordinateZ += z[planeId];
        }

        float x0 = 0;
        float y0 = 0;
        float z0 = 0;

        for(int planeId=0; planeId<m_geometry->planesPerDimension(); planeId++) {
            SimVis::TriangleCollectionVBOData p1;
            SimVis::TriangleCollectionVBOData p2;
            SimVis::TriangleCollectionVBOData p3;
            SimVis::TriangleCollectionVBOData p4;
            x0 += x[planeId];
            y0 += y[planeId];
            z0 += z[planeId];

            // First x
            p1.vertex = QVector3D(x0, 0, 0) - 0.5*QVector3D(maxPlaneCoordinateX, maxPlaneCoordinateY, maxPlaneCoordinateZ);
            p2.vertex = QVector3D(x0, 0, maxPlaneCoordinateZ) - 0.5*QVector3D(maxPlaneCoordinateX, maxPlaneCoordinateY, maxPlaneCoordinateZ);
            p3.vertex = QVector3D(x0, maxPlaneCoordinateY, maxPlaneCoordinateZ) - 0.5*QVector3D(maxPlaneCoordinateX, maxPlaneCoordinateY, maxPlaneCoordinateZ);
            p4.vertex = QVector3D(x0, maxPlaneCoordinateY, 0) - 0.5*QVector3D(maxPlaneCoordinateX, maxPlaneCoordinateY, maxPlaneCoordinateZ);
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
            p1.vertex = QVector3D(0, y0, 0) - 0.5*QVector3D(maxPlaneCoordinateX, maxPlaneCoordinateY, maxPlaneCoordinateZ);
            p2.vertex = QVector3D(0, y0, maxPlaneCoordinateZ) - 0.5*QVector3D(maxPlaneCoordinateX, maxPlaneCoordinateY, maxPlaneCoordinateZ);
            p3.vertex = QVector3D(maxPlaneCoordinateX, y0, maxPlaneCoordinateZ) - 0.5*QVector3D(maxPlaneCoordinateX, maxPlaneCoordinateY, maxPlaneCoordinateZ);
            p4.vertex = QVector3D(maxPlaneCoordinateX, y0, 0) - 0.5*QVector3D(maxPlaneCoordinateX, maxPlaneCoordinateY, maxPlaneCoordinateZ);

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
            p1.vertex = QVector3D(0, 0, z0) - 0.5*QVector3D(maxPlaneCoordinateX, maxPlaneCoordinateY, maxPlaneCoordinateZ);
            p2.vertex = QVector3D(0, maxPlaneCoordinateY, z0) - 0.5*QVector3D(maxPlaneCoordinateX, maxPlaneCoordinateY, maxPlaneCoordinateZ);
            p3.vertex = QVector3D(maxPlaneCoordinateX, maxPlaneCoordinateY, z0) - 0.5*QVector3D(maxPlaneCoordinateX, maxPlaneCoordinateY, maxPlaneCoordinateZ);
            p4.vertex = QVector3D(maxPlaneCoordinateX, 0, z0) - 0.5*QVector3D(maxPlaneCoordinateX, maxPlaneCoordinateY, maxPlaneCoordinateZ);

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
