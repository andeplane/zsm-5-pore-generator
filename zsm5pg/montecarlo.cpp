#include "montecarlo.h"
#include "random.h"
#include <QDebug>
MonteCarlo::MonteCarlo()
{

}

Zsm5geometry *MonteCarlo::geometry() const
{
    return m_geometry;
}

Statistic *MonteCarlo::statistic() const
{
    return m_statistic;
}

double MonteCarlo::computeChiSquared()
{
    double lambda = 0.76078;
    const QVector<float> &xValues = m_statistic->xValuesRaw();
    const QVector<float> &yValues = m_statistic->yValuesRaw();
    double chiSquared = 0;
    int N = xValues.size();
    for(int i=0; i<N; i++) {
        const float &x = xValues[i];
        const float &y = yValues[i];
        double targetY = lambda*exp(-lambda*(x-2.0));
        if(x<2.0) targetY = 0.0;
        double deltaYSquared = (y - targetY)*(y - targetY);
        chiSquared += deltaYSquared;
    }
    chiSquared /= N;
    return chiSquared;
}

void MonteCarlo::tick()
{
    if(!m_statistic || !m_geometry || !m_running) return;
    QVector<float> x = m_geometry->deltaXVector();
    QVector<float> y = m_geometry->deltaYVector();
    QVector<float> z = m_geometry->deltaZVector();
    float chiSquared1 = computeChiSquared();
    m_geometry->randomWalkStep(m_standardDeviation);
    m_statistic->compute(m_geometry);
    float chiSquared2 = computeChiSquared();
    // qDebug() << "csq1: " << chiSquared1 << " and csq2: " << chiSquared2;
    float deltaChiSquared = chiSquared2 - chiSquared1;
    qDebug() << deltaChiSquared;
    float w = exp(-deltaChiSquared / (m_temperature+1e-6));
    // qDebug() << "Delta chi squared: " << deltaChiSquared;
    bool accepted = deltaChiSquared < 0 || Random::nextFloat() < w;
    // bool accepted = Random::nextFloat() < w;
    // bool accepted = deltaChiSquared < 0;
    setSteps(m_steps+1);

    if(accepted) {
        setAccepted(m_accepted+1);
    } else {
        m_geometry->setDeltaXVector(x);
        m_geometry->setDeltaYVector(y);
        m_geometry->setDeltaZVector(z);
    }

    qDebug() << "Acceptance ratio: " << m_accepted / double(m_steps) << " (w: " << w << ")";
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

void MonteCarlo::setGeometry(Zsm5geometry *geometry)
{
    if (m_geometry == geometry)
        return;

    m_geometry = geometry;
    emit geometryChanged(geometry);
}

void MonteCarlo::setStatistic(Statistic *statistic)
{
    if (m_statistic == statistic)
        return;

    m_statistic = statistic;
    emit statisticChanged(statistic);
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
