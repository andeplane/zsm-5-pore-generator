#include "zsm5geometry.h"
#include "random.h"
#include <cstdlib>
Zsm5geometry::Zsm5geometry()
{

}

Zsm5geometry::~Zsm5geometry()
{
    m_deltaXVector.clear();
    m_deltaYVector.clear();
    m_deltaZVector.clear();
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
    m_deltaXVector.resize(m_planesPerDimension);
    m_deltaYVector.resize(m_planesPerDimension);
    m_deltaZVector.resize(m_planesPerDimension);
    for(int planeId=0; planeId<m_planesPerDimension; planeId++) {
        m_deltaXVector[planeId] = Random::nextFloat() * m_lengthScale;
        m_deltaYVector[planeId] = Random::nextFloat() * m_lengthScale;
        m_deltaZVector[planeId] = Random::nextFloat() * m_lengthScale;
    }
}

void Zsm5geometry::randomWalkStep(float standardDeviation)
{
    vector<float> &x = m_deltaXVector;
    vector<float> &y = m_deltaYVector;
    vector<float> &z = m_deltaZVector;

    for(int i=0; i<m_planesPerDimension; i++) {

        x[i] += Random::nextGaussianf(0, standardDeviation);
        y[i] += Random::nextGaussianf(0, standardDeviation);
        z[i] += Random::nextGaussianf(0, standardDeviation);
    }
}

float Zsm5geometry::lengthScale() const
{
    return m_lengthScale;
}

void Zsm5geometry::setLengthScale(float lengthScale)
{
    if (m_lengthScale == lengthScale)
        return;

    m_lengthScale = lengthScale;
    emit lengthScaleChanged(lengthScale);
}
