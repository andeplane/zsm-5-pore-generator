#include "statistic.h"
#include <QVariant>
#include <gsl/gsl_histogram.h>
Statistic::Statistic()
{

}

void Statistic::prepareHistogram()
{
    normalizeHistogram();
}

QVariantList Statistic::xValues() const
{
    return m_xValues;
}

QVariantList Statistic::yValues() const
{
    return m_yValues;
}

int Statistic::bins() const
{
    return m_bins;
}

void Statistic::computeHistogram()
{
    float minValue = 1e10;
    float maxValue = -1e10;

    for(int i=0; i<m_histogramValues.size(); i++) {
        minValue = std::min(m_histogramValues[i], minValue);
        maxValue = std::max(m_histogramValues[i], maxValue);
    }

    gsl_histogram *hist = gsl_histogram_alloc (m_bins);
    gsl_histogram_set_ranges_uniform (hist, minValue, maxValue);
    for(const float &value : m_histogramValues) {
        gsl_histogram_increment (hist, value);
    }

    m_xValuesRaw.resize(m_bins);
    m_yValuesRaw.resize(m_bins);
    for(int i=0; i<m_bins; i++) {
        double upper, lower;
        gsl_histogram_get_range(hist, i, &lower, &upper);
        float middle = 0.5*(upper+lower);
        float x = middle;
        float y = gsl_histogram_get(hist,i);
        m_xValuesRaw[i] = x;
        m_yValuesRaw[i] = y;
    }
    gsl_histogram_free(hist);

    normalizeHistogram();

    m_xValues.clear();
    m_yValues.clear();
    m_xValues.reserve(m_xValuesRaw.size());
    m_yValues.reserve(m_yValuesRaw.size());

    for(int i=0; i<m_xValuesRaw.size(); i++) {
        m_xValues.push_back(QVariant::fromValue<float>(m_xValuesRaw[i]));
        m_yValues.push_back(QVariant::fromValue<float>(m_yValuesRaw[i]));
    }
    emit histogramReady();
}

void Statistic::normalizeHistogram()
{
    // Normalize the histogram using the trapezoidal rule
    double integralSum = 0;

    for(int i=0; i<m_xValuesRaw.size()-1; i++) {
        float &x1 = m_xValuesRaw[i];
        float &x2 = m_xValuesRaw[i+1];
        float &y1 = m_yValuesRaw[i];
        float &y2 = m_yValuesRaw[i+1];
        double dx = x2-x1;
        double dy = y2+y1;
        integralSum += dx*dy;
    }
    integralSum *= 0.5;
    float oneOverIntegralSum = 1.0 / integralSum;
    for(int i=0; i<m_xValuesRaw.size(); i++) {
        m_yValuesRaw[i] *= oneOverIntegralSum;
    }
}

void Statistic::setXValues(QVariantList xValues)
{
    if (m_xValues == xValues)
        return;

    m_xValues = xValues;
    emit xValuesChanged(xValues);
}

void Statistic::setYValues(QVariantList yValues)
{
    if (m_yValues == yValues)
        return;

    m_yValues = yValues;
    emit yValuesChanged(yValues);
}

void Statistic::setBins(int bins)
{
    if (m_bins == bins)
        return;

    m_bins = bins;
    emit binsChanged(bins);
}
