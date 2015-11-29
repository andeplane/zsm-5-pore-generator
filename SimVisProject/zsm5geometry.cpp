#include "zsm5geometry.h"
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
        float x = rand()/double(RAND_MAX);
        float y = rand()/double(RAND_MAX);
        float z = rand()/double(RAND_MAX);
        m_planePositionsX[planeId] = x*m_planeSize;
        m_planePositionsY[planeId] = y*m_planeSize;
        m_planePositionsZ[planeId] = z*m_planeSize;
    }

    std::sort(m_planePositionsX.begin(), m_planePositionsX.end(), std::less<double>());
    std::sort(m_planePositionsY.begin(), m_planePositionsY.end(), std::less<double>());
    std::sort(m_planePositionsZ.begin(), m_planePositionsZ.end(), std::less<double>());
}
