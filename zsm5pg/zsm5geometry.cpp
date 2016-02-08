#include "zsm5geometry.h"
#include "random.h"
#include <cstdlib>
Zsm5geometry::Zsm5geometry()
{

}

Zsm5geometry::~Zsm5geometry()
{
    m_planePositionsX.clear();
    m_planePositionsY.clear();
    m_planePositionsZ.clear();
}

double Zsm5geometry::planeSize() const
{
    return m_planeSize;
}

void Zsm5geometry::setPlaneSize(double planeSize)
{
    m_planeSize = planeSize;
}

int Zsm5geometry::planesPerDimension() const
{
    return m_planesPerDimension;
}

void Zsm5geometry::setPlanesPerDimension(int planesPerDimension)
{
    m_planesPerDimension = planesPerDimension;
}

void Zsm5geometry::reset() {
    m_planePositionsX.resize(m_planesPerDimension);
    m_planePositionsY.resize(m_planesPerDimension);
    m_planePositionsZ.resize(m_planesPerDimension);
    for(int planeId=0; planeId<m_planesPerDimension; planeId++) {
        float x = Random::nextFloat();
        float y = Random::nextFloat();
        float z = Random::nextFloat();
        m_planePositionsX[planeId] = x*1.5*m_planeSize;
        m_planePositionsY[planeId] = y*1.5*m_planeSize;
        m_planePositionsZ[planeId] = z*1.5*m_planeSize;
    }

    std::sort(m_planePositionsX.begin(), m_planePositionsX.end(), std::less<double>());
    std::sort(m_planePositionsY.begin(), m_planePositionsY.end(), std::less<double>());
    std::sort(m_planePositionsZ.begin(), m_planePositionsZ.end(), std::less<double>());
}

void Zsm5geometry::randomWalkStep(float standardDeviation)
{
    vector<float> &x = m_planePositionsX;
    vector<float> &y = m_planePositionsY;
    vector<float> &z = m_planePositionsZ;

    for(int i=0; i<m_planesPerDimension; i++) {

        x[i] += Random::nextGaussianf(0, standardDeviation);
        y[i] += Random::nextGaussianf(0, standardDeviation);
        z[i] += Random::nextGaussianf(0, standardDeviation);
    }

    std::sort(m_planePositionsX.begin(), m_planePositionsX.end(), std::less<float>());
    std::sort(m_planePositionsY.begin(), m_planePositionsY.end(), std::less<float>());
    std::sort(m_planePositionsZ.begin(), m_planePositionsZ.end(), std::less<float>());
}
