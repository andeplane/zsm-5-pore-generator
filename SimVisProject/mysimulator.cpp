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
    m_wantedDistribution.resize(m_settings.distributionSize);
    float avgLengthPerBox = m_settings.planeSize / m_settings.planesPerDimension;
    float maxLength = 5.0*avgLengthPerBox;
    float dx = maxLength / m_distribution.size();
    float sigma = 0.1*maxLength;
    float mu = 0.3*maxLength;
    for(int i=0; i<m_wantedDistribution.size(); i++) {
        float x = dx*i;
        float p = 1.0/(sigma*sqrt(2*M_PI))*exp(-(x-mu)*(x-mu)/(2.0*sigma*sigma));
        m_wantedDistribution[i].setX(x);
        m_wantedDistribution[i].setY(p);
    }
}

double MyWorker::meanSquareError() {
    double error = 0;
    for(int i=0; i<m_distribution.size(); i++) {
        double delta = m_distribution[i].y() - m_wantedDistribution[i].y();
        error += delta*delta;
    }

    return error;
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
    computeVolume();
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
        mySimulator->distribution()->setPoints(m_distribution);
        mySimulator->wantedDistribution()->setPoints(m_wantedDistribution);
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
    }
}

void MyWorker::work()
{

    using namespace SimVis;

}

void MyWorker::computeVolume()
{
    volume = 0;
    m_volumes.resize(m_settings.planesPerDimension-1, vector<vector<double> >(m_settings.planesPerDimension-1, vector<double>(m_settings.planesPerDimension-1, 0)));
    m_distribution.resize(1024);
    for(int i=0; i<m_settings.planesPerDimension-1; i++) {
        double x1 = m_planePositionsX[i];
        double x2 = m_planePositionsX[i+1];
        double dx = x2-x1;
        for(int j=0; j<m_settings.planesPerDimension-1; j++) {
            double y1 = m_planePositionsY[j];
            double y2 = m_planePositionsY[j+1];
            double dy = y2-y1;
            for(int k=0; k<m_settings.planesPerDimension-1; k++) {
                double z1 = m_planePositionsZ[k];
                double z2 = m_planePositionsZ[k+1];
                double dz = z2-z1;
                double volumeBox = dx*dy*dz;
                m_volumes[i][j][k] = volumeBox;
                volume += volumeBox;
            }
        }
    }
    updateDistribution();
}

void MyWorker::updateDistribution()
{
    m_distribution.resize(m_settings.distributionSize);

    double avgLengthPerBox = m_settings.planeSize / m_settings.planesPerDimension;
    double maxLength = 5.0*avgLengthPerBox;
    double dx = maxLength / m_distribution.size();
    double numberOfVolumes = powf(m_settings.planesPerDimension-1,3);

    for(int i=0; i<m_distribution.size(); i++) {
        QPointF &p = m_distribution[i];
        p.setX(i*dx);
        p.setY(0);
    }

    for(int i=0; i<m_settings.planesPerDimension-1; i++) {
        for(int j=0; j<m_settings.planesPerDimension-1; j++) {
            for(int k=0; k<m_settings.planesPerDimension-1; k++) {
                double volume = m_volumes[i][j][k];
                double length = powf(volume, 1.0/3.0);
                int histogramIndex = length / dx;
                if(histogramIndex < m_distribution.size()) {
                    QPointF &p = m_distribution[histogramIndex];
                    p.setY(p.y()+1);
                }
            }
        }
    }

    for(QPointF &p : m_distribution) {
        p.setY(p.y()/(numberOfVolumes*dx));
    }
}
