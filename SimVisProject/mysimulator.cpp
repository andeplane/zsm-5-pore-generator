#include "mysimulator.h"
#include <QDebug>
MySimulator::MySimulator()
{

}

int MySimulator::planesPerDimension() const
{
    return m_settings.planesPerDimension;
}

void MySimulator::setPlanesPerDimension(int planesPerDimension)
{
    if (m_settings.planesPerDimension == planesPerDimension)
        return;

    m_settings.planesPerDimension = planesPerDimension;
    emit planesPerDimensionChanged(planesPerDimension);
}

SimulatorWorker *MySimulator::createWorker()
{
    return new MyWorker();
}

MyWorker::MyWorker()
{
    using namespace SimVis;
    reset();
}

void MyWorker::reset() {
    m_vertices.clear();
    m_planePositionsX.resize(m_settings.planesPerDimension);
    m_planePositionsY.resize(m_settings.planesPerDimension);
    m_planePositionsZ.resize(m_settings.planesPerDimension);
    for(int planeId=0; planeId<m_settings.planesPerDimension; planeId++) {
        float x = rand()/double(RAND_MAX);
        float y = rand()/double(RAND_MAX);
        float z = rand()/double(RAND_MAX);
        m_planePositionsX[planeId] = x*m_settings.planeSize;
        m_planePositionsY[planeId] = y*m_settings.planeSize;
        m_planePositionsZ[planeId] = z*m_settings.planeSize;
    }

    std::sort(m_planePositionsX.begin(), m_planePositionsX.end(), std::less<double>());
    std::sort(m_planePositionsY.begin(), m_planePositionsY.end(), std::less<double>());
    std::sort(m_planePositionsZ.begin(), m_planePositionsZ.end(), std::less<double>());

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
    }
}

void MyWorker::synchronizeRenderer(Renderable *renderableObject)
{
    // Synchronize with renderables.
    TriangleCollection* triangleCollection = qobject_cast<TriangleCollection*>(renderableObject);
    if(triangleCollection) {
        triangleCollection->data.clear();
        // Update triangle collection renderable. Similarly if you use other renderables.
        float planeSize = m_settings.planeSize;

        for(int planeId=0; planeId<m_settings.planesPerDimension; planeId++) {
            SimVis::TriangleCollectionVBOData p1;
            SimVis::TriangleCollectionVBOData p2;
            SimVis::TriangleCollectionVBOData p3;
            SimVis::TriangleCollectionVBOData p4;
            // First x
            p1.vertex = QVector3D(m_planePositionsX[planeId], 0, 0) - 0.5*QVector3D(planeSize, planeSize, planeSize);
            p2.vertex = QVector3D(m_planePositionsX[planeId], 0, planeSize) - 0.5*QVector3D(planeSize, planeSize, planeSize);
            p3.vertex = QVector3D(m_planePositionsX[planeId], planeSize, planeSize) - 0.5*QVector3D(planeSize, planeSize, planeSize);
            p4.vertex = QVector3D(m_planePositionsX[planeId], planeSize, 0) - 0.5*QVector3D(planeSize, planeSize, planeSize);
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
            p1.vertex = QVector3D(0, m_planePositionsY[planeId], 0) - 0.5*QVector3D(planeSize, planeSize, planeSize);
            p2.vertex = QVector3D(0, m_planePositionsY[planeId], planeSize) - 0.5*QVector3D(planeSize, planeSize, planeSize);
            p3.vertex = QVector3D(planeSize, m_planePositionsY[planeId], planeSize) - 0.5*QVector3D(planeSize, planeSize, planeSize);
            p4.vertex = QVector3D(planeSize, m_planePositionsY[planeId], 0) - 0.5*QVector3D(planeSize, planeSize, planeSize);

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
            p1.vertex = QVector3D(0, 0, m_planePositionsY[planeId]) - 0.5*QVector3D(planeSize, planeSize, planeSize);
            p2.vertex = QVector3D(0, planeSize, m_planePositionsY[planeId]) - 0.5*QVector3D(planeSize, planeSize, planeSize);;
            p3.vertex = QVector3D(planeSize, planeSize, m_planePositionsY[planeId]) - 0.5*QVector3D(planeSize, planeSize, planeSize);;
            p4.vertex = QVector3D(planeSize, 0, m_planePositionsY[planeId]) - 0.5*QVector3D(planeSize, planeSize, planeSize);;

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
        return;
    }
}

void MyWorker::work()
{

    using namespace SimVis;

}
