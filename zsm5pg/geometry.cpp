#include "geometry.h"
#include "random.h"
#include "inifile.h"
#include <cstdlib>
#include <QDebug>
#include <QFile>
#include <QUrl>
#include <QFileInfo>
Geometry::Geometry()
{

}

Geometry::~Geometry()
{
    m_deltaXVector.clear();
    m_deltaYVector.clear();
    m_deltaZVector.clear();
    QString fileName = QString("%1/geometry.txt").arg(m_filePath);
    save(fileName);
}

bool Geometry::shouldBeRejected() {
    QVector<float> &x = deltaXVector();
    QVector<float> &y = deltaYVector();
    QVector<float> &z = deltaZVector();

    for(int i=0; i<x.size(); i++) {
        const float dx = x[i];
        for(int j=0; j<y.size(); j++) {
            const float dy = y[j];
            for(int k=0; k<z.size(); k++) {
                const float dz = z[k];
                float H1 = dx;
                float H2 = dy;
                float H3 = dz;

                if(H2 < H1) std::swap(H2, H1);
                if(H3 < H1) std::swap(H3, H1);
                if(H3 < H2) std::swap(H3, H2);

                if(H1< 2 || H1> 4.5) return true;
                if(H2 > 19) return true;
                if(H3 > 19) return true;
            }
        }
    }
    return false;
}

void Geometry::loadIniFile(IniFile *iniFile)
{
    QFileInfo fileInfo(QString("%1/geometry.txt").arg(m_filePath));
    int planesPerDimension = iniFile->getInt("planesPerDimension");
    bool loadPrevious = iniFile->getBool("loadPrevious") && fileInfo.exists();
    setRandomWalkFraction(iniFile->getDouble("randomWalkFraction"));
    setIsValid(true);
    setMode(iniFile->getInt("mode"));
    setVerbose(iniFile->getBool("verbose"));

    if(loadPrevious) {
        load(QString("%1/geometry.txt").arg(m_filePath));
        if(this->planesPerDimension() != planesPerDimension) {
            if(m_verbose) qDebug() << "Resizing from " << this->planesPerDimension() << " planes per dimension to " << planesPerDimension;
            resize(planesPerDimension);
        }
    } else {
        setPlanesPerDimension(planesPerDimension);
        reset(2, 19);
    }

    qDebug() << "Geometry loaded ini file with ";
    qDebug() << "  Mode: " << m_mode;
    qDebug() << "  Verbose: " << m_verbose;
    qDebug() << "  Planes per dimension: " << m_planesPerDimension;
    qDebug() << "  Load previous: " << (loadPrevious ? "true" : "false");
    qDebug() << "  Random walk fraction: " << m_randomWalkFraction;
}

void Geometry::setVerbose(bool verbose)
{
    if (m_verbose == verbose)
        return;

    m_verbose = verbose;
    emit verboseChanged(verbose);
}

int Geometry::planesPerDimension() const
{
    return m_planesPerDimension;
}

void Geometry::setPlanesPerDimension(int planesPerDimension)
{
    m_planesPerDimension = planesPerDimension;
}

void Geometry::reset(float min, float max) {
    if(m_verbose) qDebug() << "Resetting with min: " << min << " and max: " << max << " and planes per dim: " << m_planesPerDimension;
    m_deltaXVector.resize(m_planesPerDimension);
    m_deltaYVector.resize(m_planesPerDimension);
    m_deltaZVector.resize(m_planesPerDimension);

    if(m_mode == 0) {
        float delta = max - min;
        for(int planeId=0; planeId<m_planesPerDimension; planeId++) {
//            m_deltaXVector[planeId] = 2.0*min + Random::nextFloat()*delta;
//            m_deltaYVector[planeId] = 2.0*min + Random::nextFloat()*delta;
//            m_deltaZVector[planeId] = 2.0*min + Random::nextFloat()*delta;
            m_deltaXVector[planeId] = 3.0;
            m_deltaYVector[planeId] = 3.0;
            m_deltaZVector[planeId] = 3.0;
            if(m_verbose) qDebug() << "Plane (" << planeId << ", X): " << m_deltaXVector[planeId];
            if(m_verbose) qDebug() << "Plane (" << planeId << ", Y): " << m_deltaYVector[planeId];
            if(m_verbose) qDebug() << "Plane (" << planeId << ", Z): " << m_deltaZVector[planeId];
        }
    } else {
        for(int planeId=0; planeId<m_planesPerDimension; planeId++) {
            m_deltaXVector[planeId] = Random::nextInt(int(min),int(max));
            m_deltaYVector[planeId] = Random::nextInt(int(min), int(max));
            m_deltaZVector[planeId] = Random::nextInt(int(min), int(max));
//            m_deltaXVector[planeId] = Random::nextInt(int(18),int(19));
//            m_deltaYVector[planeId] = Random::nextInt(int(18), int(19));
//            m_deltaZVector[planeId] = Random::nextInt(int(18), int(19));
            if(m_verbose) qDebug() << "Plane " << (3*planeId+0) <<": " << m_deltaXVector[planeId];
            if(m_verbose) qDebug() << "Plane " << (3*planeId+1) <<": " << m_deltaYVector[planeId];
            if(m_verbose) qDebug() << "Plane " << (3*planeId+2) <<": " << m_deltaZVector[planeId];
        }
    }
}

bool Geometry::randomWalkStep(float standardDeviation)
{
    bool anyChanges = false;
    if(m_mode == 0) {
        double Hmin = 2;
        double Hmax = 19;
        for(int planeId=0; planeId<m_planesPerDimension; planeId++) {
            float dx = Random::nextGaussianf(0, standardDeviation);
            float dy = Random::nextGaussianf(0, standardDeviation);
            float dz = Random::nextGaussianf(0, standardDeviation);

            if(Random::nextFloat() < 0.05) {
                anyChanges = true;
                m_deltaXVector[planeId] = Random::nextDouble(Hmin, Hmax);
            } else if(m_deltaXVector[planeId] + dx > Hmin && m_deltaXVector[planeId] + dx < Hmax && Random::nextFloat() < m_randomWalkFraction) {
                anyChanges = true;
                m_deltaXVector[planeId] += dx;
            }

            if(Random::nextFloat() < 0.05) {
                anyChanges = true;
                m_deltaYVector[planeId] = Random::nextDouble(Hmin, Hmax);
            } else if(m_deltaYVector[planeId] + dy > Hmin && m_deltaYVector[planeId] + dy < Hmax && Random::nextFloat() < m_randomWalkFraction) {
                anyChanges = true;
                m_deltaYVector[planeId] += dy;
            }

            if(Random::nextFloat() < 0.05) {
                anyChanges = true;
                m_deltaZVector[planeId] = Random::nextDouble(Hmin, Hmax);
            } else if(m_deltaZVector[planeId] + dz > Hmin && m_deltaZVector[planeId] + dz < Hmax && Random::nextFloat() < m_randomWalkFraction) {
                anyChanges = true;
                m_deltaZVector[planeId] += dz;
            }
//            if(m_verbose) qDebug() << "Changed plane (" << planeId << ", X): " << m_deltaXVector[planeId];
//            if(m_verbose) qDebug() << "Changed plane (" << planeId << ", Y): " << m_deltaYVector[planeId];
//            if(m_verbose) qDebug() << "Changed plane (" << planeId << ", Z): " << m_deltaZVector[planeId];
        }
    } else {
        for(int planeId=0; planeId<m_planesPerDimension; planeId++) {
            int dx = Random::nextInt(-3,3);
            if(Random::nextFloat() < 0.05) dx = Random::nextInt(-10, 10);
            int dy = Random::nextInt(-3,3);
            if(Random::nextFloat() < 0.05) dy = Random::nextInt(-10, 10);
            int dz = Random::nextInt(-3,3);
            if(Random::nextFloat() < 0.05) dz = Random::nextInt(-10, 10);
            if(m_deltaXVector[planeId] + dx >= 2 && m_deltaXVector[planeId] + dx < 20 && Random::nextFloat() < m_randomWalkFraction) {
                anyChanges = true;
                m_deltaXVector[planeId] += dx;
                // qDebug() << "Changed " << 3*planeId+0 << " to " << m_deltaXVector[planeId];
            }
            if(m_deltaYVector[planeId] + dy >= 2 && m_deltaYVector[planeId] + dy < 20 && Random::nextFloat() < m_randomWalkFraction) {
                anyChanges = true;
                m_deltaYVector[planeId] += dy;
                // qDebug() << "Changed " << 3*planeId+1 << " to " << m_deltaYVector[planeId];
            }
            if(m_deltaZVector[planeId] + dz >= 2 && m_deltaZVector[planeId] + dz < 20 && Random::nextFloat() < m_randomWalkFraction) {
                anyChanges = true;
                m_deltaZVector[planeId] += dz;
                // qDebug() << "Changed " << 3*planeId+2 << " to " << m_deltaZVector[planeId];
            }

//            if(m_verbose) qDebug() << "Changed plane " << (3*planeId+0) <<": " << m_deltaXVector[planeId];
//            if(m_verbose) qDebug() << "Changed plane " << (3*planeId+1) <<": " << m_deltaYVector[planeId];
//            if(m_verbose) qDebug() << "Changed plane " << (3*planeId+2) <<": " << m_deltaZVector[planeId];
        }
    }

    return anyChanges;
}

void Geometry::saveState(QFile &file)
{
    QTextStream stream(&file);
    stream << "rwFraction = " << m_randomWalkFraction << endl;
}

void Geometry::loadState(IniFile *iniFile) {
    setRandomWalkFraction(iniFile->getDouble("rwFraction"));
}

void Geometry::generateFromPSD(QVector<qreal> probabilities)
{
    QVector<qreal> cumulative = probabilities;

    // First normalize
    double sum = 0.0;
    for(double p : probabilities) {
        sum += p;
    }
    double normalizeFactor = 1.0 / sum;

    sum = 0.0;
    for(int i=0; i<probabilities.size(); i++) {
        probabilities[i]*= normalizeFactor;
        sum += probabilities[i];
        cumulative[i] = sum;
    }

    // Now loop through each plane and draw random number

    for(int i=0; i<m_planesPerDimension; i++) {
        double rnd = Random::nextDouble();
        for(int j=0; j<cumulative.size(); j++) {
            if(rnd < cumulative[j]) {
                double delta = j+1;
                m_deltaXVector[i] = delta;
                qDebug() << "dx[" << i << "] = " << delta;
                break;
            }
        }

        rnd = Random::nextDouble();
        for(int j=0; j<cumulative.size(); j++) {
            if(rnd < cumulative[j]) {
                double delta = j+1;
                m_deltaYVector[i] = delta;
                qDebug() << "dy[" << i << "] = " << delta;
                break;
            }
        }

        rnd = Random::nextDouble();
        for(int j=0; j<cumulative.size(); j++) {
            if(rnd < cumulative[j]) {
                double delta = j+1;
                m_deltaZVector[i] = delta;
                qDebug() << "dz[" << i << "] = " << delta;
                break;
            }
        }
    }
}

void Geometry::resize(int newNumberOfPlanes) {
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

float Geometry::randomWalkFraction() const
{
    return m_randomWalkFraction;
}

int Geometry::mode() const
{
    return m_mode;
}

QString Geometry::filePath() const
{
    return m_filePath;
}

bool Geometry::isValid() const
{
    return m_isValid;
}

bool Geometry::verbose() const
{
    return m_verbose;
}

void Geometry::save(QString fileName)
{
    //QFile file(QUrl(filename).toLocalFile());
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Could not open file "+fileName;
        return;
    }

    QTextStream out(&file);
    out << "# number of planes per dimension\n" << m_planesPerDimension << "\n";
    for(int i=0; i<m_planesPerDimension; i++) {
        out << m_deltaXVector[i] << " " << m_deltaYVector[i] << " " << m_deltaZVector[i] << "\n";
    }
    file.close();
}

void Geometry::load(QString filename)
{
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
            setPlanesPerDimension(QString(words[0]).toFloat(&castOk));
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

QVector<float> &Geometry::deltaXVector() { return m_deltaXVector; }

QVector<float> &Geometry::deltaYVector() { return m_deltaYVector; }

QVector<float> &Geometry::deltaZVector() { return m_deltaZVector; }

void Geometry::setDeltaXVector(const QVector<float> &deltaX) { m_deltaXVector = deltaX; }

void Geometry::setDeltaYVector(const QVector<float> &deltaY) { m_deltaYVector = deltaY; }

void Geometry::setDeltaZVector(const QVector<float> &deltaZ) { m_deltaZVector = deltaZ; }

float Geometry::lengthScale() const
{
    return m_lengthScale;
}

double Geometry::totalVolume()
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

void Geometry::setLengthScale(float lengthScale)
{
    if (m_lengthScale == lengthScale)
        return;

    m_lengthScale = lengthScale;
    emit lengthScaleChanged(lengthScale);
}

void Geometry::setRandomWalkFraction(float randomWalkFraction)
{
    if (m_randomWalkFraction == randomWalkFraction)
        return;

    m_randomWalkFraction = randomWalkFraction;
    emit randomWalkFractionChanged(randomWalkFraction);
}

void Geometry::setMode(int mode)
{
    if (m_mode == mode)
        return;

    m_mode = mode;
    emit modeChanged(mode);
}

void Geometry::setFilePath(QString filePath)
{
    if (m_filePath == filePath)
        return;

    m_filePath = filePath;
    emit filePathChanged(filePath);
}

void Geometry::setIsValid(bool isValid)
{
    if (m_isValid == isValid)
        return;

    m_isValid = isValid;
    emit isValidChanged(isValid);
}
