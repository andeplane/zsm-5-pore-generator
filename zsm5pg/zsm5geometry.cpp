#include "zsm5geometry.h"
#include "random.h"
#include <cstdlib>
#include <QDebug>
#include <QFile>
#include <QUrl>
Zsm5geometry::Zsm5geometry()
{

}

Zsm5geometry::~Zsm5geometry()
{
    m_deltaXVector.clear();
    m_deltaYVector.clear();
    m_deltaZVector.clear();
}

int Zsm5geometry::planesPerDimension() const
{
    return m_planesPerDimension;
}

void Zsm5geometry::setPlanesPerDimension(int planesPerDimension)
{
    m_planesPerDimension = planesPerDimension;
}

void Zsm5geometry::reset(float min, float max) {
    m_deltaXVector.resize(m_planesPerDimension);
    m_deltaYVector.resize(m_planesPerDimension);
    m_deltaZVector.resize(m_planesPerDimension);

    float delta = max - min;
    if(m_mode == 0) {
        for(int planeId=0; planeId<m_planesPerDimension; planeId++) {
            m_deltaXVector[planeId] = 2.0*min + Random::nextFloat()*delta;
            m_deltaYVector[planeId] = 2.0*min + Random::nextFloat()*delta;
            m_deltaZVector[planeId] = 2.0*min + Random::nextFloat()*delta;
        }
    } else {
        for(int planeId=0; planeId<m_planesPerDimension; planeId++) {
            m_deltaXVector[planeId] = Random::nextInt(18,19);
            m_deltaYVector[planeId] = Random::nextInt(18,19);
            m_deltaZVector[planeId] = Random::nextInt(18,19);
        }
    }
}

void Zsm5geometry::randomWalkStep(float standardDeviation)
{
    if(m_mode == 0) {
    for(int i=0; i<m_planesPerDimension; i++) {
        float dx = Random::nextGaussianf(0, standardDeviation);
        float dy = Random::nextGaussianf(0, standardDeviation);
        float dz = Random::nextGaussianf(0, standardDeviation);
        if(m_deltaXVector[i] + dx > 2 && m_deltaXVector[i] + dx < 20 && Random::nextFloat() < m_randomWalkFraction) m_deltaXVector[i] += dx;
        if(m_deltaYVector[i] + dy > 2 && m_deltaYVector[i] + dy < 20 && Random::nextFloat() < m_randomWalkFraction) m_deltaYVector[i] += dy;
        if(m_deltaZVector[i] + dz > 2 && m_deltaZVector[i] + dz < 20 && Random::nextFloat() < m_randomWalkFraction) m_deltaZVector[i] += dz;
    }
    } else {
        // qDebug() << "planes = [";
        for(int i=0; i<m_planesPerDimension; i++) {
            int dx = Random::nextInt(-1,1);
            int dy = Random::nextInt(-1,1);
            int dz = Random::nextInt(-1,1);
            if(m_deltaXVector[i] + dx > 2 && m_deltaXVector[i] + dx < 20 && Random::nextFloat() < m_randomWalkFraction) m_deltaXVector[i] += dx;
            if(m_deltaYVector[i] + dy > 2 && m_deltaYVector[i] + dy < 20 && Random::nextFloat() < m_randomWalkFraction) m_deltaYVector[i] += dy;
            if(m_deltaZVector[i] + dz > 2 && m_deltaZVector[i] + dz < 20 && Random::nextFloat() < m_randomWalkFraction) m_deltaZVector[i] += dz;
            // qDebug() << m_deltaXVector[i] << " " << m_deltaYVector[i] << " " << m_deltaZVector[i];
        }
        // qDebug() << "];";

    }
}

void Zsm5geometry::resize(int newNumberOfPlanes) {
    if(newNumberOfPlanes > m_planesPerDimension) {
        int delta = newNumberOfPlanes - m_planesPerDimension;
        m_deltaXVector.resize(m_planesPerDimension + delta);
        m_deltaYVector.resize(m_planesPerDimension + delta);
        m_deltaZVector.resize(m_planesPerDimension + delta);
        for(int i=0; i<delta; i++) {
            int index = m_planesPerDimension+i;
            int moddedIndex = index % m_planesPerDimension;
            m_deltaXVector[index] = m_deltaXVector[moddedIndex];
            m_deltaYVector[index] = m_deltaYVector[moddedIndex];
            m_deltaZVector[index] = m_deltaZVector[moddedIndex];
        }
    } else {
        m_deltaXVector.resize(newNumberOfPlanes);
        m_deltaYVector.resize(newNumberOfPlanes);
        m_deltaZVector.resize(newNumberOfPlanes);
    }

    setPlanesPerDimension(newNumberOfPlanes);
}

float Zsm5geometry::randomWalkFraction() const
{
    return m_randomWalkFraction;
}

int Zsm5geometry::mode() const
{
    return m_mode;
}

void Zsm5geometry::save(QString filename)
{
    //QFile file(QUrl(filename).toLocalFile());
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Could not open file "+filename;
        return;
    }

    QTextStream out(&file);
    out << "# number of planes per dimension\n" << m_planesPerDimension << "\n";
    for(int i=0; i<m_planesPerDimension; i++) {
        out << m_deltaXVector[i] << " " << m_deltaYVector[i] << " " << m_deltaZVector[i] << "\n";
    }
    file.close();
}

void Zsm5geometry::load(QString filename)
{
    //QFile file(QUrl(filename).toLocalFile());
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Could not open file "+filename;
        return;
    }
    QTextStream in(&file);
    int numberOfXYZValuesRead = 0;
    while (!in.atEnd()) {
        QString line = in.readLine();
        if(line.trimmed().startsWith("#")) continue;

        QStringList words = line.split(" ");
        if(words.count() ==1) {
            bool castOk;
            m_planesPerDimension = QString(words[0]).toFloat(&castOk);
            m_deltaXVector.resize(m_planesPerDimension);
            m_deltaYVector.resize(m_planesPerDimension);
            m_deltaZVector.resize(m_planesPerDimension);
        }

        if(words.length() != 3) continue;

        bool castOk;
        float dx = QString(words[0]).toFloat(&castOk);
        if(!castOk) continue;
        float dy = QString(words[1]).toFloat(&castOk);
        if(!castOk) continue;
        float dz = QString(words[2]).toFloat(&castOk);
        if(!castOk) continue;
        m_deltaXVector[numberOfXYZValuesRead] = dx;
        m_deltaYVector[numberOfXYZValuesRead] = dy;
        m_deltaZVector[numberOfXYZValuesRead] = dz;
        numberOfXYZValuesRead++;
    }
}

QVector<float> &Zsm5geometry::deltaXVector() { return m_deltaXVector; }

QVector<float> &Zsm5geometry::deltaYVector() { return m_deltaYVector; }

QVector<float> &Zsm5geometry::deltaZVector() { return m_deltaZVector; }

void Zsm5geometry::setDeltaXVector(const QVector<float> &deltaX) { m_deltaXVector = deltaX; }

void Zsm5geometry::setDeltaYVector(const QVector<float> &deltaY) { m_deltaYVector = deltaY; }

void Zsm5geometry::setDeltaZVector(const QVector<float> &deltaZ) { m_deltaZVector = deltaZ; }

float Zsm5geometry::lengthScale() const
{
    return m_lengthScale;
}

double Zsm5geometry::totalVolume()
{
    double L[3];
    L[0] = 0; L[1] = 0; L[2] = 0;

    for(int i=0; i<m_deltaXVector.size(); i++) {
        L[0] += m_deltaXVector[i];
        L[1] += m_deltaXVector[i];
        L[2] += m_deltaXVector[i];
    }
    return L[0]*L[1]*L[2];
}

void Zsm5geometry::setLengthScale(float lengthScale)
{
    if (m_lengthScale == lengthScale)
        return;

    m_lengthScale = lengthScale;
    emit lengthScaleChanged(lengthScale);
}

void Zsm5geometry::setRandomWalkFraction(float randomWalkFraction)
{
    if (m_randomWalkFraction == randomWalkFraction)
        return;

    m_randomWalkFraction = randomWalkFraction;
    emit randomWalkFractionChanged(randomWalkFraction);
}

void Zsm5geometry::setMode(int mode)
{
    if (m_mode == mode)
        return;

    m_mode = mode;
    emit modeChanged(mode);
}
