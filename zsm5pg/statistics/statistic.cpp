#include "statistic.h"
#include <QVariant>
#include <QDebug>
#include <gsl/gsl_histogram.h>
#include <QFile>
#include <cmath>
Statistic::Statistic()
{
    m_lineSeries = new QLineSeries();
    m_name = "Statistic";
}

void Statistic::compute(Zsm5geometry *geometry)
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

const QVector<float> &Statistic::xValuesRaw() const
{
    return m_xValuesRaw;
}

const QVector<float> &Statistic::yValuesRaw() const
{
    return m_yValuesRaw;
}

QVariantList Statistic::yValues() const
{
    return m_yValues;
}

int Statistic::bins() const
{
    return m_bins;
}

float Statistic::min() const
{
    return m_min;
}

float Statistic::max() const
{
    return m_max;
}

void Statistic::save(QString filename)
{
    QFile file(filename);
    if(!file.open(QFileDevice::WriteOnly | QFileDevice::Text)) {
        qDebug() << "Error, could not not save file " << filename;
        return;
    }

    QTextStream stream(&file);
    for(int i=0; i<m_xValuesRaw.count(); i++) {
        float x = m_xValuesRaw[i];
        float y = m_yValuesRaw[i];
        stream << x << " " << y << "\n";
    }
    file.close();
}

void Statistic::load(QString filename)
{
    QFile file(filename);
    if(!file.open(QFileDevice::ReadOnly | QFileDevice::Text)) {
        qDebug() << "Could not open file " << filename;
        exit(1);
    }

    m_xValuesRaw.clear();
    m_yValuesRaw.clear();

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        line = line.trimmed();
        QStringList splitted = line.split(" ");
        if(splitted.count()==2) {
            bool ok;
            float x = splitted[0].toFloat(&ok);
            if(ok) {
                float y = splitted[1].toFloat(&ok);
                if(ok) {
                    m_xValuesRaw.push_back(x);
                    m_yValuesRaw.push_back(y);
                }
            } else {
                qDebug() << "Warning, could not parse line " << line;
            }
        }
    }
}

void Statistic::emitReady()
{
    emit histogramReady();
}

void Statistic::createLineSeries() {
    m_lineSeries->clear();
    for(int bin=0; bin<m_bins; bin++) {
        float x = m_xValuesRaw[bin];
        float y = m_yValuesRaw[bin];
        m_lineSeries->append(x,y);
    }
}

double Statistic::eval(double x)
{
    for(int i=0; i<m_xValuesRaw.size()-1; i++) {
        double x0 = m_xValuesRaw[i];
        double x1 = m_xValuesRaw[i+1];
        if(x0 <= x && x <= x1) {
            double y0 = m_yValuesRaw[i];
            double y1 = m_yValuesRaw[i+1];
            double delta = x1 - x0;
            double f = (x1 - x) / delta;
            double value = y0*f + (1.0 - f)*y1;
            return value;
        }
    }
    qDebug() << m_name << " could not interpolate x=" << x;
    qDebug() << "X values: " << m_xValuesRaw;
    exit(1);
}

double Statistic::chiSquared(Statistic *statistic)
{
    double chiSquared = 0;
    for(int bin = 0; bin<m_xValuesRaw.size(); bin++) {
        double x = m_xValuesRaw[bin];
        double y_this = m_yValuesRaw[bin];
        double y_other = statistic->eval(x);
        double delta = y_this - y_other;
        chiSquared += delta*delta;
    }
    return chiSquared;
}

void Statistic::computeHistogram()
{
    gsl_histogram *hist = gsl_histogram_alloc (m_bins);
    gsl_histogram_set_ranges_uniform (hist, m_min, m_max);
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
        if(isnan(x) || isnan(y)) {
            qDebug() << "Statistic::computeHistogram() nan: " << x << ", " << y;
        }
        m_xValuesRaw[i] = x;
        m_yValuesRaw[i] = y;
    }
    gsl_histogram_free(hist);
    normalizeHistogram();
}

void Statistic::updateQML() {
    m_xValues.clear();
    m_yValues.clear();
    m_xValues.reserve(m_xValuesRaw.size());
    m_yValues.reserve(m_yValuesRaw.size());

    for(int i=0; i<m_xValuesRaw.size(); i++) {
        m_xValues.push_back(QVariant::fromValue<float>(m_xValuesRaw[i]));
        m_yValues.push_back(QVariant::fromValue<float>(m_yValuesRaw[i]));
        // qDebug() << "Updating QML with " << m_xValuesRaw[i] << ", " << m_yValuesRaw[i];
    }
    emit histogramReady();
}

QString Statistic::xLabel() const
{
    return m_xLabel;
}

QString Statistic::yLabel() const
{
    return m_yLabel;
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

void Statistic::setYValuesRaw(const QVector<float> &yValuesRaw)
{
    m_yValuesRaw = yValuesRaw;
}

QLineSeries *Statistic::lineSeries() const
{
    return m_lineSeries;
}

void Statistic::setXValuesRaw(const QVector<float> &xValuesRaw)
{
    m_xValuesRaw = xValuesRaw;
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

void Statistic::setMin(float min)
{
    if (m_min == min)
        return;

    m_min = min;
    emit minChanged(min);
}

void Statistic::setMax(float max)
{
    if (m_max == max)
        return;
    m_max = max;
    emit maxChanged(max);
}

void Statistic::setLineSeries(QLineSeries *lineSeries)
{
    if (m_lineSeries == lineSeries)
        return;

    m_lineSeries = lineSeries;
    emit lineSeriesChanged(lineSeries);
}

void Statistic::setXLabel(QString xLabel)
{
    if (m_xLabel == xLabel)
        return;

    m_xLabel = xLabel;
    emit xLabelChanged(xLabel);
}

void Statistic::setYLabel(QString yLabel)
{
    if (m_yLabel == yLabel)
        return;

    m_yLabel = yLabel;
    emit yLabelChanged(yLabel);
}
