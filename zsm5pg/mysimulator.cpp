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

Geometry *MySimulator::geometry() const
{
    return m_geometry;
}

Statistic *MySimulator::statistic() const
{
    return m_statistic;
}

MonteCarlo *MySimulator::monteCarlo() const
{
    return m_monteCarlo;
}

float MySimulator::tickTime() const
{
    return m_tickTime;
}

void MySimulator::setTime(double time)
{
    if (m_time == time)
        return;

    m_time = time;
    emit timeChanged(time);
}

void MySimulator::setGeometry(Geometry *geometry)
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

void MySimulator::setMonteCarlo(MonteCarlo *monteCarlo)
{
    if (m_monteCarlo == monteCarlo)
        return;

    m_monteCarlo = monteCarlo;
    emit monteCarloChanged(monteCarlo);
}

void MySimulator::setTickTime(float tickTime)
{
    if (m_tickTime == tickTime)
        return;

    m_tickTime = tickTime;
    emit tickTimeChanged(tickTime);
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
        m_monteCarlo = mySimulator->monteCarlo();
        mySimulator->setTickTime(double(m_timeElapsed)/(m_numberOfTicks+1)/1000.0);
        if(m_geometry && mySimulator->m_reset) {
            m_geometry->reset(1, 5);
            mySimulator->m_reset = false;
        }
        mySimulator->setTime(mySimulator->time()+1.0);
        if(m_statistic) {
            m_statistic->emitReady();
        }
    }
}

void MyWorker::synchronizeRenderer(Renderable *renderableObject)
{
    // Synchronize with renderables.
    TriangleCollection* triangleCollection = qobject_cast<TriangleCollection*>(renderableObject);
    if(triangleCollection && m_geometry) {
        triangleCollection->data.clear();
        // Update triangle collection renderable. Similarly if you use other renderables.
        QVector<float> &x = m_geometry->deltaXVector();
        QVector<float> &y = m_geometry->deltaYVector();
        QVector<float> &z = m_geometry->deltaZVector();
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
    if(m_statistic && m_geometry) m_statistic->compute(m_geometry);
    if(m_monteCarlo) {
        if(!m_monteCarlo->running()) return;
        QElapsedTimer timer;
        timer.start();

        for(int i=0; i<10; i++) {
            m_monteCarlo->tick();
        }

        m_timeElapsed += timer.elapsed();
        m_numberOfTicks++;
    }
}
