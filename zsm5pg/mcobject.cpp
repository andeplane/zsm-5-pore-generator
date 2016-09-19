#include "mcobject.h"
#include <random.h>
MCObject::MCObject(QObject *parent) : QObject(parent)
{

}

double MCObject::value() const
{
    return m_value;
}

double MCObject::standardDeviation() const
{
    return m_standardDeviation;
}

void MCObject::randomWalk()
{
    storeCurrent();
    double delta = Random::nextGaussian(0, m_standardDeviation);
    setValue(value() + delta);
}

void MCObject::storeCurrent()
{
    m_valueStored = m_value;
}

void MCObject::rejectRW()
{
    setValue(m_valueStored);
}

void MCObject::setValue(double value)
{
    if (m_value == value)
        return;

    m_value = value;
    emit valueChanged(value);
}

void MCObject::setStandardDeviation(double standardDeviation)
{
    if (m_standardDeviation == standardDeviation)
        return;

    m_standardDeviation = standardDeviation;
    emit standardDeviationChanged(standardDeviation);
}
