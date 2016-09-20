#include "mcobject.h"
#include "inifile.h"
#include <QTextStream>
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

void MCObject::saveState(QFile &file)
{
    QTextStream stream(&file);
    stream << QString("%1_value = %2").arg(m_name).arg(m_value) << endl;
}

void MCObject::loadState(IniFile *iniFile)
{
    setValue(iniFile->getDouble(QString("%1_value").arg(m_name)));
}

QString MCObject::name() const
{
    return m_name;
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

void MCObject::setName(QString name)
{
    if (m_name == name)
        return;

    m_name = name;
    emit nameChanged(name);
}
