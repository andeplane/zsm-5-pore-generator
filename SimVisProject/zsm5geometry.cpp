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

void Zsm5geometry::followGradient(Zsm5geometry &gradient)
{
    vector<float> &x = m_planePositionsX;
    vector<float> &y = m_planePositionsY;
    vector<float> &z = m_planePositionsZ;

    vector<float> &dEdx = gradient.planePositionsX();
    vector<float> &dEdy = gradient.planePositionsY();
    vector<float> &dEdz = gradient.planePositionsZ();

    float eps = 1e-3;
    for(int i=0; i<m_planesPerDimension; i++) {
        x[i] -= dEdx[i]*eps;
        y[i] -= dEdy[i]*eps;
        z[i] -= dEdz[i]*eps;
    }

    std::sort(m_planePositionsX.begin(), m_planePositionsX.end(), std::less<double>());
    std::sort(m_planePositionsY.begin(), m_planePositionsY.end(), std::less<double>());
    std::sort(m_planePositionsZ.begin(), m_planePositionsZ.end(), std::less<double>());
}
