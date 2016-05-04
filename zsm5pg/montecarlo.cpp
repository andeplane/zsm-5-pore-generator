#include "montecarlo.h"
#include "random.h"
#include <QDebug>
void MonteCarlo::setDebug(bool debug)
{
    m_debug = debug;
}

MonteCarlo::MonteCarlo()
{

}

Zsm5geometry *MonteCarlo::geometry() const
{
    return m_geometry;
}

void MonteCarlo::tick()
{
    if(!m_model || !m_data || !m_geometry || !m_running) return;
    QVector<float> x = m_geometry->deltaXVector();
    QVector<float> y = m_geometry->deltaYVector();
    QVector<float> z = m_geometry->deltaZVector();

    QVector<float> xValues = m_model->xValuesRaw();
    QVector<float> yValues = m_model->yValuesRaw();

    float chiSquared1 = m_model->chiSquared(m_data);
    m_geometry->randomWalkStep(m_standardDeviation);
    m_model->compute(m_geometry);
    float chiSquared2 = m_model->chiSquared(m_data);
    float deltaChiSquared = chiSquared2 - chiSquared1;

    float w = exp(-deltaChiSquared / (m_temperature+1e-6));

    bool accepted = deltaChiSquared < 0 || Random::nextFloat() < w;
    // bool accepted = Random::nextFloat() < w;
    // bool accepted = deltaChiSquared < 0;
    setSteps(m_steps+1);

    if(accepted) {
        setAccepted(m_accepted+1);
        setChiSquared(chiSquared2);

        if(m_debug) qDebug() << "csq1: " << chiSquared1 << " and csq2: " << chiSquared2;
        if(m_debug) qDebug() << "Delta chi squared: " << deltaChiSquared;
        if(m_debug) qDebug() << "Accepted: " << accepted  << " (w = " << w << ")";
    } else {
        m_geometry->setDeltaXVector(x);
        m_geometry->setDeltaYVector(y);
        m_geometry->setDeltaZVector(z);
        m_model->setXValuesRaw(xValues);
        m_model->setYValuesRaw(yValues);
//        qDebug() << "Chi squared before: " << chiSquared1;
//        chiSquared2 = m_model->chiSquared(m_data);
//        qDebug() << "Chi squared now: " << chiSquared2;
//        chiSquared2 = m_model->chiSquared(m_data);
//        qDebug() << "Chi squared again: " << chiSquared2;
        setChiSquared(chiSquared1);
    }

    // qDebug() << "Acceptance ratio: " << m_accepted / double(m_steps) << " (w: " << w << ")";
}

float MonteCarlo::standardDeviation() const
{
    return m_standardDeviation;
}

float MonteCarlo::temperature() const
{
    return m_temperature;
}

int MonteCarlo::steps() const
{
    return m_steps;
}

int MonteCarlo::accepted() const
{
    return m_accepted;
}

bool MonteCarlo::running() const
{
    return m_running;
}

float MonteCarlo::acceptanceRatio()
{
    return m_accepted / float(m_steps);
}

float MonteCarlo::chiSquared() const
{
    return m_chiSquared;
}

Statistic *MonteCarlo::model() const
{
    return m_model;
}

Statistic *MonteCarlo::data() const
{
    return m_data;
}

void MonteCarlo::setGeometry(Zsm5geometry *geometry)
{
    if (m_geometry == geometry)
        return;

    m_geometry = geometry;
    emit geometryChanged(geometry);
}

void MonteCarlo::setStandardDeviation(float standardDeviation)
{
    if (m_standardDeviation == standardDeviation)
        return;

    m_standardDeviation = standardDeviation;
    emit standardDeviationChanged(standardDeviation);
}

void MonteCarlo::setTemperature(float temperature)
{
    if (m_temperature == temperature)
        return;

    m_temperature = temperature;
    emit temperatureChanged(temperature);
}

void MonteCarlo::setSteps(int steps)
{
    if (m_steps == steps)
        return;

    m_steps = steps;
    emit stepsChanged(steps);
}

void MonteCarlo::setAccepted(int accepted)
{
    if (m_accepted == accepted)
        return;

    m_accepted = accepted;
    emit acceptedChanged(accepted);
}

void MonteCarlo::setRunning(bool running)
{
    if (m_running == running)
        return;

    m_running = running;
    emit runningChanged(running);
}

void MonteCarlo::setModel(Statistic *model)
{
    if (m_model == model)
        return;

    m_model = model;
    emit modelChanged(model);
}

void MonteCarlo::setData(Statistic *data)
{
    if (m_data == data)
        return;

    m_data = data;
    emit dataChanged(data);
}

void MonteCarlo::setChiSquared(float chiSquared)
{
    if (m_chiSquared == chiSquared)
        return;

    m_chiSquared = chiSquared;
    emit chiSquaredChanged(chiSquared);
}
