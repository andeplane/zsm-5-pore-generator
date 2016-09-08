#include "montecarlo.h"
#include "random.h"
#include <QDebug>
void MonteCarlo::setDebug(bool debug)
{
    m_debug = debug;
}

float MonteCarlo::targetAcceptanceRatio() const
{
    return m_targetAcceptanceRatio;
}

float MonteCarlo::acceptanceRatio()
{
    return m_acceptanceRatio;
}

MonteCarlo::MonteCarlo()
{

}

PlaneGeometry *MonteCarlo::geometry() const
{
    return m_geometry;
}

void MonteCarlo::tick()
{
    if(!m_model || !m_data || !m_geometry || !m_running) return;
    if(m_debug) qDebug() << "Starting monte carlo step. Current chi squared: " << m_model->chiSquared(m_data);
    QVector<float> x = m_geometry->deltaXVector();
    QVector<float> y = m_geometry->deltaYVector();
    QVector<float> z = m_geometry->deltaZVector();

    QList<QPointF> points = m_model->points();

    float chiSquared1 = m_model->chiSquared(m_data);
    if(m_debug) qDebug() << "Performing random walk step with std dev: " << m_standardDeviation << " and random walk fraction: " << m_geometry->randomWalkFraction();
    m_geometry->randomWalkStep(m_standardDeviation);
    m_model->compute(m_geometry);
    float chiSquared2 = m_model->chiSquared(m_data);
    float deltaChiSquared = chiSquared2 - chiSquared1;
    if(m_debug) qDebug() << "New chi squared: " << chiSquared2 << " with deltaChiSquared: " << deltaChiSquared;

    bool accepted = deltaChiSquared < 0 || Random::nextFloat() < exp(-deltaChiSquared / m_temperature);
    setSteps(m_steps+1);

    if(accepted) {
        setAccepted(m_accepted+1);
        setChiSquared(chiSquared2);
        if(m_debug) qDebug() << "     STEP ACCEPTED";
        writeToFile();
    } else {
        m_geometry->setDeltaXVector(x);
        m_geometry->setDeltaYVector(y);
        m_geometry->setDeltaZVector(z);
        m_model->setPoints(points);
        setChiSquared(chiSquared1);
        if(m_debug) qDebug() << "     STEP REJECTED";
    }
    if(m_steps % 20 == 0) {
        updateRandomWalkFraction();
    }
}

void MonteCarlo::writeToFile() {
    if(m_filename.isEmpty()) return;
    if(!m_file.isOpen()) {
        m_file.setFileName(m_filename);
        if(m_debug) qDebug() << "Opening MC file " << m_filename;
        if (!m_file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
            qDebug() << "Error, could not open monte carlo file " << m_filename;
            exit(1);
        }
        QTextStream out(&m_file);
        out.setFieldWidth(12);
        out.setFieldAlignment(QTextStream::AlignLeft);
        out << "#step" << "chiSq" << "AcceptanceRatio" << "RWFraction" << "\n";
    }

    QTextStream out(&m_file);
    out.setFieldWidth(8);
    out.setFieldAlignment(QTextStream::AlignLeft);
    out << m_accepted << " " << m_chiSquared << " " << " " << m_acceptanceRatio << " " << m_geometry->randomWalkFraction() << "\n";
}

void MonteCarlo::updateRandomWalkFraction() {
    float instantaneousAcceptanceRatio = m_accepted / float(m_steps);
    float acceptanceRatioLowpass = acceptanceRatio()*0.9 + 0.1*instantaneousAcceptanceRatio;
    setAcceptanceRatio(acceptanceRatioLowpass);
    float factor = sqrt(1.0 - m_acceptanceRatioAdjustmentTimeScale*(m_targetAcceptanceRatio/acceptanceRatioLowpass - 1.0));
    float newRandomWalkFraction = m_geometry->randomWalkFraction()*factor;
    if(newRandomWalkFraction>1) newRandomWalkFraction = 1.0;
    if(m_debug) {
        qDebug() << "  Updating random walk fraction. Target acceptance ratio is " << m_targetAcceptanceRatio << ", current: " << m_acceptanceRatio;
        qDebug() << "  New random walk fraction (factor: " << factor << "): " << newRandomWalkFraction << "(old: " << m_geometry->randomWalkFraction() << ")";
    }
    m_geometry->setRandomWalkFraction(newRandomWalkFraction);
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

float MonteCarlo::acceptanceRatioAdjustmentTimeScale() const
{
    return m_acceptanceRatioAdjustmentTimeScale;
}

QString MonteCarlo::filename() const
{
    return m_filename;
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

void MonteCarlo::setGeometry(PlaneGeometry *geometry)
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

void MonteCarlo::setTargetAcceptanceRatio(float targetAcceptanceRatio)
{
    if (m_targetAcceptanceRatio == targetAcceptanceRatio)
        return;

    m_targetAcceptanceRatio = targetAcceptanceRatio;
    emit targetAcceptanceRatioChanged(targetAcceptanceRatio);
}

void MonteCarlo::setAcceptanceRatio(float acceptanceRatio)
{
    if (m_acceptanceRatio == acceptanceRatio)
        return;

    m_acceptanceRatio = acceptanceRatio;
    emit acceptanceRatioChanged(acceptanceRatio);
}

void MonteCarlo::setAcceptanceRatioAdjustmentTimeScale(float acceptanceRatioAdjustmentTimeScale)
{
    if (m_acceptanceRatioAdjustmentTimeScale == acceptanceRatioAdjustmentTimeScale)
        return;

    m_acceptanceRatioAdjustmentTimeScale = acceptanceRatioAdjustmentTimeScale;
    emit acceptanceRatioAdjustmentTimeScaleChanged(acceptanceRatioAdjustmentTimeScale);
}

void MonteCarlo::setFilename(QString filename)
{
    if (m_filename == filename)
        return;

    m_filename = filename;
    emit filenameChanged(filename);
}
