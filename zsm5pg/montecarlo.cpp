#include "inifile.h"
#include "montecarlo.h"
#include "random.h"
#include <QDebug>
void MonteCarlo::setDebug(bool debug)
{
    m_verbose = debug;
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

Geometry *MonteCarlo::geometry() const
{
    return m_geometry;
}

void MonteCarlo::tick(int step)
{
    if(!isValid()) {
        qDebug() << "Error, MonteCarlo not ready...";
        exit(1);
    }
    QVector<float> x = m_geometry->deltaXVector();
    QVector<float> y = m_geometry->deltaYVector();
    QVector<float> z = m_geometry->deltaZVector();

    QList<QList<QPointF>> points;
    float chiSquared1 = 0;
    for(int i=0; i<m_models.size(); i++) {
        Statistic *model = m_models.at(i).value<Statistic*>();
        Statistic *data = m_datas.at(i).value<Statistic*>();
        model->compute(m_geometry, step);
        chiSquared1 += model->chiSquared(data);
        points.push_back(model->points());
    }

    if(m_verbose) qDebug() << "Starting monte carlo step. Current chi squared: " << chiSquared1;
    if(m_verbose) qDebug() << "Performing random walk step with std dev: " << m_standardDeviation << " and random walk fraction: " << m_geometry->randomWalkFraction();

    bool anyChanges = m_geometry->randomWalkStep(m_standardDeviation);
    if(!anyChanges) {
        return;
    }

    float chiSquared2 = 0;
    for(int i=0; i<m_models.size(); i++) {
        Statistic *model = m_models.at(i).value<Statistic*>();
        Statistic *data = m_datas.at(i).value<Statistic*>();
        model->setDirty();
        model->compute(m_geometry, step);
        chiSquared2 += model->chiSquared(data);
    }

    float deltaChiSquared = chiSquared2 - chiSquared1;
    if(m_verbose) qDebug() << "New chi squared: " << chiSquared2 << " with deltaChiSquared: " << deltaChiSquared;

    bool accepted = deltaChiSquared < 0 || Random::nextFloat() < exp(-deltaChiSquared / m_temperature);
    setSteps(m_steps+1);

    if(accepted) {
        setAccepted(m_accepted+1);
        setChiSquared(chiSquared2);
        if(m_verbose) qDebug() << "     STEP ACCEPTED";
        writeToFile();
    } else {
        m_geometry->setDeltaXVector(x);
        m_geometry->setDeltaYVector(y);
        m_geometry->setDeltaZVector(z);
        for(int i=0; i<m_models.size(); i++) {
            Statistic *model = m_models.at(i).value<Statistic*>();
            model->setPoints(points.at(i));
        }
        setChiSquared(chiSquared1);
        if(m_verbose) qDebug() << "     STEP REJECTED";
    }
    if(m_steps % 20 == 0) {
        updateRandomWalkFraction();
    }
    points.clear();
}

void MonteCarlo::loadIniFile(IniFile *iniFile)
{
    setStandardDeviation(iniFile->getDouble("mcStandardDeviation"));
    setTemperature(iniFile->getDouble("mcTemperature"));
    setFilename(QString("%1/%2").arg(m_filePath).arg(iniFile->getString("mcFilename")));
    setVerbose(iniFile->getBool("verbose"));
    qDebug() << "MonteCarlo loaded ini file with ";
    qDebug() << "  Verbose: " << m_verbose;
    qDebug() << "  Standard deviation: " << m_standardDeviation;
    qDebug() << "  Temperature: " << m_temperature;
    qDebug() << "  Filename: " << m_filename;
}

void MonteCarlo::setVerbose(bool verbose)
{
    if (m_verbose == verbose)
            return;

        m_verbose = verbose;
        emit verboseChanged(verbose);
}

void MonteCarlo::writeToFile() {
    if(m_filename.isEmpty()) return;
    if(!m_file.isOpen()) {
        m_file.setFileName(m_filename);
        if(m_verbose) qDebug() << "Opening MC file " << m_filename;
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
    if(isnan(newRandomWalkFraction)) newRandomWalkFraction = m_geometry->randomWalkFraction();

    if(newRandomWalkFraction>1) newRandomWalkFraction = 1.0;
    // Minimum fraction is to move 1 plane
    float minimumRandomWalkFraction = 1.0 / (3*m_geometry->planesPerDimension());

    if(newRandomWalkFraction<minimumRandomWalkFraction) newRandomWalkFraction = minimumRandomWalkFraction;
    if(m_verbose) {
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

bool MonteCarlo::isValid() const
{
    return m_geometry && m_models.size()==m_datas.size();
}

float MonteCarlo::acceptanceRatioAdjustmentTimeScale() const
{
    return m_acceptanceRatioAdjustmentTimeScale;
}

QString MonteCarlo::filename() const
{
    return m_filename;
}

QVariantList MonteCarlo::models() const
{
    return m_models;
}

QVariantList MonteCarlo::datas() const
{
    return m_datas;
}

QString MonteCarlo::filePath() const
{
    return m_filePath;
}

bool MonteCarlo::verbose() const
{
    return m_verbose;
}

float MonteCarlo::chiSquared() const
{
    return m_chiSquared;
}

void MonteCarlo::setGeometry(Geometry *geometry)
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

void MonteCarlo::setModels(QVariantList models)
{
    if (m_models == models)
        return;

    m_models = models;
    emit modelsChanged(models);
}

void MonteCarlo::setDatas(QVariantList datas)
{
    if (m_datas == datas)
        return;

    m_datas = datas;
    emit datasChanged(datas);
}

void MonteCarlo::setFilePath(QString filePath)
{
    if (m_filePath == filePath)
        return;

    m_filePath = filePath;
    emit filePathChanged(filePath);
}
