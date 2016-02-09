#include "zsm5geometry.h"
#include "random.h"
#include <cstdlib>
#include <QDebug>
#include <QFile>

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

void Zsm5geometry::reset() {
    float mean = 3.0;
    m_deltaXVector.resize(m_planesPerDimension);
    m_deltaYVector.resize(m_planesPerDimension);
    m_deltaZVector.resize(m_planesPerDimension);

    for(int planeId=0; planeId<m_planesPerDimension; planeId++) {
        m_deltaXVector[planeId] = 2.0 + Random::nextExponentialf(0.76078);
        m_deltaYVector[planeId] = 2.0 + Random::nextExponentialf(0.76078);
        m_deltaZVector[planeId] = 2.0 + Random::nextExponentialf(0.76078);
//        m_deltaXVector[planeId] = std::max(2.0,Random::nextGaussianf(mean, m_lengthScale));
//        m_deltaYVector[planeId] = std::max(2.0,Random::nextGaussianf(mean, m_lengthScale));
//        m_deltaZVector[planeId] = std::max(2.0,Random::nextGaussianf(mean, m_lengthScale));
//        m_deltaXVector[planeId] = Random::nextFloat(0.1, m_lengthScale);
//        m_deltaYVector[planeId] = Random::nextFloat(0.1, m_lengthScale);
//        m_deltaZVector[planeId] = Random::nextFloat(0.1, m_lengthScale);
    }
}

void Zsm5geometry::randomWalkStep(float standardDeviation)
{
    for(int i=0; i<m_planesPerDimension; i++) {
        float dx = Random::nextGaussianf(0, standardDeviation);
        float dy = Random::nextGaussianf(0, standardDeviation);
        float dz = Random::nextGaussianf(0, standardDeviation);
        if(m_deltaXVector[i] + dx > 0) m_deltaXVector[i] += dx;
        if(m_deltaYVector[i] + dy > 0) m_deltaYVector[i] += dy;
        if(m_deltaZVector[i] + dz > 0) m_deltaZVector[i] += dz;
    }
    m_dirty = true;
}

void Zsm5geometry::save(QString filename)
{
    QFile file(QUrl(filename).toLocalFile());
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
        QFile file(QUrl(filename).toLocalFile());
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
            m_deltaYVector[numberOfXYZValuesRead] = dx;
            m_deltaZVector[numberOfXYZValuesRead] = dx;
            numberOfXYZValuesRead++;
        }
        m_dirty = true;
}

float Zsm5geometry::lengthScale() const
{
    return m_lengthScale;
}

bool Zsm5geometry::dirty() const
{
    return m_dirty;
}

void Zsm5geometry::setLengthScale(float lengthScale)
{
    if (m_lengthScale == lengthScale)
        return;

    m_lengthScale = lengthScale;
    emit lengthScaleChanged(lengthScale);
}

void Zsm5geometry::setDirty(bool dirty)
{
    if (m_dirty == dirty)
        return;

    m_dirty = dirty;
    emit dirtyChanged(dirty);
}
