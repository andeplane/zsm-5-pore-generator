#include "distributionstatistic.h"
#include <cmath>
#include <QDebug>
void DistributionStatistic::setType(DistributionStatistic::Type type, float xMin, float xMax, int points)
{
    m_type = Type::None; // This won't trigger update on setBins, setMax, setMin.
    setBins(points);
    setMax(xMax);
    setMin(xMin);
    m_type = type;

    emit typeChanged(); // Will trigger update
}

DistributionStatistic::DistributionStatistic() {
    connect(this, &DistributionStatistic::maxChanged, this, &DistributionStatistic::update);
    connect(this, &DistributionStatistic::minChanged, this, &DistributionStatistic::update);
    connect(this, &DistributionStatistic::typeChanged, this, &DistributionStatistic::update);
    connect(this, &DistributionStatistic::binsChanged, this, &DistributionStatistic::update);
}

DistributionStatistic::Type DistributionStatistic::type() const
{
    return m_type;
}

float DistributionStatistic::normalDistributionMean() const
{
    return m_normalDistributionMean;
}

void DistributionStatistic::setNormalDistributionMean(float normalDistributionMean)
{
    m_normalDistributionMean = normalDistributionMean;
}

float DistributionStatistic::normalDistributionStandardDeviation() const
{
    return m_normalDistributionStandardDeviation;
}

void DistributionStatistic::setNormalDistributionStandardDeviation(float normalDistributionStandardDeviation)
{
    m_normalDistributionStandardDeviation = normalDistributionStandardDeviation;
}

float DistributionStatistic::exponentialDistributionMean() const
{
    return m_exponentialDistributionMean;
}

void DistributionStatistic::setExponentialDistributionMean(float exponentialDistributionMean)
{
    m_exponentialDistributionMean = exponentialDistributionMean;
}

void DistributionStatistic::update() {
    if(m_type == Type::None) return;
    m_xValuesRaw.resize(bins());
    m_yValuesRaw.resize(bins());

    float dx = (max() - min()) / (bins() - 1);
    for(int bin = 0; bin < bins(); bin++) {
        float x = min() + bin*dx;
        m_xValuesRaw[bin] = x;
    }

    if(m_type == Type::Normal) {
        float sigma = m_normalDistributionStandardDeviation;
        float mu = m_normalDistributionMean;

        for(int bin = 0; bin < bins(); bin++) {
            float x = m_xValuesRaw[bin];
            float y = 1.0 / (sigma*sqrt(2*M_PI)) * exp(-(x-mu)*(x-mu) / (2*sigma*sigma));

            m_yValuesRaw[bin] = y;
        }
    }

    if(m_type == Type::Exponential) {
        float lambda = m_exponentialDistributionMean;

        for(int bin = 0; bin < bins(); bin++) {
            float x = m_xValuesRaw[bin];
            float y = lambda * exp(-lambda*x);

            m_yValuesRaw[bin] = y;
        }
    }
}
