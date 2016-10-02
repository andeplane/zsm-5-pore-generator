#include "statistic.h"
#include <QVariant>
#include <QDebug>
#include <gsl/gsl_histogram.h>
#include <QFile>
#include <cmath>
#include "inifile.h"
Statistic::Statistic(QObject *parent) : QObject(parent),
    m_histogramAverageCount(100)
{
    m_name = "Statistic";
}

Statistic::~Statistic()
{
    QString fileName = QString("%1/%2.txt").arg(m_filePath).arg(m_name);
    save(fileName);
}

void Statistic::compute(Geometry *geometry, int timestep)
{
    if(m_constant) return;
    if(m_lastComputed == timestep) return;

    if(m_timesteps >= m_histogramAverageCount) {
        // we should start removing values in the beginning of the list
        int valuesPerTimestep = m_histogramValues.size() / m_histogramAverageCount;
        m_histogramValues.erase(m_histogramValues.begin(), m_histogramValues.begin() + valuesPerTimestep);
    }
    m_timesteps++;
    m_lastComputed = timestep;
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

void Statistic::save(QString fileName)
{
    qDebug() << "Saving with filename " << fileName;
    QFile file(fileName);
    if(!file.open(QFileDevice::WriteOnly | QFileDevice::Text)) {
        qDebug() << "Error, could not not save file " << fileName;
        return;
    }

    QTextStream stream(&file);
    for(int i=0; i<m_points.count(); i++) {
        float x = m_points[i].x();
        float y = m_points[i].y();
        stream << x << " " << y << "\n";
    }
    file.close();
}

void Statistic::saveHistogramValues(QUrl filename)
{
    QFile file(filename.toLocalFile());
    if(!file.open(QFileDevice::WriteOnly | QFileDevice::Text)) {
        qDebug() << "Error, could not not save file " << filename;
        return;
    }
    qDebug() << "Saved histogram values to " << filename;

    QTextStream stream(&file);
    for(int i=0; i<m_histogramValues.count(); i++) {
        stream << m_histogramValues[i] << " ";
    }
    stream << "\n";
    file.close();
}

void Statistic::load(QString fileName)
{
    QFile file(fileName);
    if(!file.open(QFileDevice::ReadOnly | QFileDevice::Text)) {
        qDebug() << "Could not open file " << fileName;
        exit(1);
    }

    m_points.clear();

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
                    m_points.push_back(QPointF(x,y));
                }
            } else {
                qDebug() << "Warning, could not parse line " << line;
            }
        }
    }

    qDebug() << "  loaded " << m_points.size() << " points from " << fileName;
}

void Statistic::emitReady()
{
    emit histogramReady();
}

double Statistic::eval(double x, bool &ok)
{
    for(int i=0; i<m_points.size()-1; i++) {
        double x0 = m_points[i].x();
        double x1 = m_points[i+1].x();
        if(x0 <= x && x <= x1) {
            double y0 = m_points[i].y();
            double y1 = m_points[i+1].y();
            double delta = x1 - x0;
            double f = (x1 - x) / delta;
            double value = y0*f + (1.0 - f)*y1;
            ok = true;
            return value;
        }
    }
    ok = false;
    return 0;
//    qDebug() << m_name << " could not interpolate x=" << x;
//    qDebug() << "X values: " << m_points;
//    exit(1);
}

double Statistic::chiSquared(Statistic *statistic)
{
    double chiSquared = 0;
    for(int bin = 0; bin<m_points.size(); bin++) {
        bool ok;
        double x = m_points[bin].x();
        double y_this = m_points[bin].y();
        double y_other = statistic->eval(x, ok);
        if(ok) {
            double delta = (y_this - y_other) / (y_other + std::numeric_limits<double>::min());
            if(y_other==0 || y_this==0) continue; // Skip values being zero
            chiSquared += delta*delta;
        }
    }
    return chiSquared;
}

void Statistic::computeHistogram()
{
    // Produce histogram
    gsl_histogram *hist = gsl_histogram_alloc (m_bins);
    gsl_histogram_set_ranges_uniform (hist, m_min, m_max);
    for(const float &value : m_histogramValues) {
        gsl_histogram_increment (hist, value);
    }

    // Copy data over to list over points for plotting
    m_points.clear();
    m_points.reserve(m_bins);

    for(int i=0; i<m_bins; i++) {
        double upper, lower;
        gsl_histogram_get_range(hist, i, &lower, &upper);
        float middle = 0.5*(upper+lower);
        float x = middle;
        float y = gsl_histogram_get(hist,i);
        if(std::isnan(x) || std::isnan(y)) {
            qDebug() << "Statistic::computeHistogram() nan: " << x << ", " << y;
        }
        m_points.push_back(QPointF(x,y));
    }
    gsl_histogram_free(hist);
    normalizeHistogram();
}

void Statistic::updateQML() {
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

QString Statistic::name() const
{
    return m_name;
}

QList<QPointF> &Statistic::points()
{
    return m_points;
}

void Statistic::normalizeHistogram()
{
    // Normalize the histogram using the trapezoidal rule
    double integralSum = 0;

    for(int i=0; i<m_points.size()-1; i++) {
        const qreal &x1 = m_points[i].x();
        const qreal &x2 = m_points[i+1].x();
        const qreal &y1 = m_points[i].y();
        const qreal &y2 = m_points[i+1].y();
        double dx = x2-x1;
        double dy = y2+y1;
        integralSum += dx*dy;
    }
    integralSum *= 0.5;
    float oneOverIntegralSum = 1.0 / integralSum;
    for(int i=0; i<m_points.size(); i++) {
        float normalizedValue = m_points[i].y() * oneOverIntegralSum;
        m_points[i].setY(normalizedValue);
    }
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

void Statistic::updateSeries(QAbstractSeries *series) {
    if(series) {
        QXYSeries *xySeries = static_cast<QXYSeries*>(series);
        if(xySeries) {
            xySeries->replace(m_points);
        } else {
            qDebug() << "Warning, tried to update a non QXYSeries series";
        }
    }
}

int Statistic::histogramAverageCount() const
{
    return m_histogramAverageCount;
}

int Statistic::mode() const
{
    return m_mode;
}

bool Statistic::isValid() const
{
    return m_isValid && m_points.size();
}

QString Statistic::sourceKey() const
{
    return m_sourceKey;
}

QString Statistic::filePath() const
{
    return m_filePath;
}

bool Statistic::constant() const
{
    return m_constant;
}

void Statistic::setDirty()
{
    m_lastComputed = -1;
}

void Statistic::setName(QString name)
{
    if (m_name == name)
        return;

    qDebug() << "Setting name: " << name;
    m_name = name;
    emit nameChanged(name);
}

void Statistic::setPoints(QList<QPointF> points)
{
    if (m_points == points)
        return;

    m_points = points;
    emit pointsChanged(points);
}

void Statistic::setHistogramAverageCount(int histogramAverageCount)
{
    if (m_histogramAverageCount == histogramAverageCount)
        return;

    m_histogramAverageCount = histogramAverageCount;
    emit histogramAverageCountChanged(histogramAverageCount);
}

void Statistic::setMode(int mode)
{
    if (m_mode == mode)
        return;

    m_mode = mode;
    emit modeChanged(mode);
}

void Statistic::loadIniFile(IniFile *iniFile) {
    setMin(iniFile->getDouble("xMin"));
    setMax(iniFile->getDouble("xMax"));
    setBins(iniFile->getInt("bins"));
    setMode(iniFile->getInt("mode"));

    setHistogramAverageCount(iniFile->getInt("histogramAverageCount"));
    qDebug() << "Statistic loaded ini file with ";
    qDebug() << "  Min: " << m_min;
    qDebug() << "  Max: " << m_max;
    qDebug() << "  Bins: " << m_bins;
    qDebug() << "  Mode: " << m_mode;

    if(m_sourceKey.isEmpty()) return;
    // QString fileName = QString("%1/%2").arg(m_filePath).arg(iniFile->getString(m_sourceKey));
    QString fileName = iniFile->getString(m_sourceKey);
    load(fileName);
    setIsValid(true);
    qDebug() << "  Filename: " << fileName;
}

void Statistic::setIsValid(bool isValid)
{
    if (m_isValid == isValid)
        return;

    m_isValid = isValid;
    emit isValidChanged(isValid);
}

void Statistic::setSourceKey(QString sourceKey)
{
    if (m_sourceKey == sourceKey)
        return;

    m_sourceKey = sourceKey;
    emit sourceKeyChanged(sourceKey);
}

void Statistic::setFilePath(QString filePath)
{
    if (m_filePath == filePath)
        return;

    m_filePath = filePath;
    emit filePathChanged(filePath);
}

void Statistic::setConstant(bool constant)
{
    if (m_constant == constant)
        return;

    m_constant = constant;
    emit constantChanged(constant);
}
