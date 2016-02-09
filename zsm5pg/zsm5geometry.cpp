#include "zsm5geometry.h"
#include "random.h"
#include <cstdlib>
#include <QDebug>

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
    float mean = 2.5;
    m_deltaXVector.resize(m_planesPerDimension);
    m_deltaYVector.resize(m_planesPerDimension);
    m_deltaZVector.resize(m_planesPerDimension);
    for(int planeId=0; planeId<m_planesPerDimension; planeId++) {
        m_deltaXVector[planeId] = Random::nextGaussianf(mean, m_lengthScale);
        m_deltaYVector[planeId] = Random::nextGaussianf(mean, m_lengthScale);
        m_deltaZVector[planeId] = Random::nextGaussianf(mean, m_lengthScale);
//        m_deltaXVector[planeId] = Random::nextFloat(0.1, m_lengthScale);
//        m_deltaYVector[planeId] = Random::nextFloat(0.1, m_lengthScale);
//        m_deltaZVector[planeId] = Random::nextFloat(0.1, m_lengthScale);
    }
}

void Zsm5geometry::randomWalkStep(float standardDeviation)
{
    for(int i=0; i<m_planesPerDimension; i++) {
        float dx = Random::nextGaussianf(0, standardDeviation);
        float dy = Random::nextGaussianf(0, standardDeviation);
        float dz = Random::nextGaussianf(0, standardDeviation);
        if(m_deltaXVector[i] + dx > 0) m_deltaXVector[i] += dx;
        if(m_deltaYVector[i] + dy > 0) m_deltaYVector[i] += dy;
        if(m_deltaZVector[i] + dz > 0) m_deltaZVector[i] += dz;
    }
     m_dirty = true;
}

float Zsm5geometry::lengthScale() const
{
    return m_lengthScale;
}

bool Zsm5geometry::dirty() const
{
    return m_dirty;
}

void Zsm5geometry::setLengthScale(float lengthScale)
{
    if (m_lengthScale == lengthScale)
        return;

    m_lengthScale = lengthScale;
    emit lengthScaleChanged(lengthScale);
}

void Zsm5geometry::setDirty(bool dirty)
{
    if (m_dirty == dirty)
        return;

    m_dirty = dirty;
    emit dirtyChanged(dirty);
}
