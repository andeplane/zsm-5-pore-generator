#include "concentration.h"
#include <QFile>
#include <QDebug>
#include <cmath>
#include <QTextStream>
#include "../zsm5geometry.h"
Concentration::Concentration(QObject *parent) : Statistic(parent)
{

}

void Concentration::readFile() {
    QFile file(m_fileName);
    if(!file.open(QFileDevice::ReadOnly | QFileDevice::Text)) {
        qDebug() << "Could not find adsorption matrix file " << m_fileName;
        exit(1);
    }
    m_values.resize(20); // We don't use index 0 in this because it is nice to use the H values as lookup index (they start on 1)
    m_volumes.resize(20);
    QTextStream in(&file);
    while (!in.atEnd()) {

        QString line = in.readLine().trimmed();
        QStringList splitted = line.split(" ");
        if(splitted.size() == 4) {
            QString poreSize = splitted[0];
            QString pressure = splitted[1];
            QString N_adsorbed = splitted[2];
            QString poreVolume = splitted[3];

            bool ok;
            float H = poreSize.toInt(&ok);
            if(!ok) { qDebug() << "Could not parse pore size."; exit(1); }
            float P = pressure.toFloat(&ok);
            if(!ok) { qDebug() << "Could not parse pressure."; exit(1); }
            float N = N_adsorbed.toFloat(&ok);
            if(!ok) { qDebug() << "Could not parse adsorbed count."; exit(1); }
            float V = poreVolume.toFloat(&ok);
            if(!ok) { qDebug() << "Could not parse pore volume."; exit(1); }

            if(H==1) m_pressures.push_back(P);

            m_values[H].push_back(N);
            m_volumes[H] = V;
        }
    }

    m_name = "Concentration";
    m_xLabel = "Pressure [p/p0]";
    m_yLabel = "V_ads/cm^3";
}

void Concentration::computeMode0(Geometry *geometry) {
    if(!geometry) return;

    QVector<float> &x = geometry->deltaXVector();
    QVector<float> &y = geometry->deltaYVector();
    QVector<float> &z = geometry->deltaZVector();

    QVector<float> numberOfAdsorbedAtoms;
    numberOfAdsorbedAtoms.resize(m_pressures.size());
    for(float &v : numberOfAdsorbedAtoms) {
        v = 0.0;
    }

    int outside = 0;
    int inside = 0;
    for(int i=0; i<x.size(); i++) {
        const float dx = x[i];
        for(int j=0; j<y.size(); j++) {
            const float dy = y[j];
            for(int k=0; k<z.size(); k++) {
                const float dz = z[k];
                float poreSize = std::min(std::min(dx, dy), dz);
                const float poreVolume = dx*dy*dz;
#ifdef POREISCBRT
                poreSize = cbrt(poreVolume);
#endif
                // int H = std::roundf(poreSize);

                float H = poreSize;  // 1

                int H0 = poreSize; // 19
                int H1 = int(poreSize)+1;  // 20
                float dH = H1-H0;  // 1
                float fraction = (H1-H) / dH;  // (20-19.5) / 1 = 0.5
                if(H1>19) {
                    fraction = 1.0; // 1.0
                    H1 = 19; // force this
                }

                if(H>=2 && H<=19) {
                    for(int pIndex=0; pIndex<m_pressures.size(); pIndex++) {
                        float N_ads0 = m_values[H0][pIndex]/m_volumes[H0]*poreVolume;
                        float N_ads1 = m_values[H1][pIndex]/m_volumes[H1]*poreVolume;
                        float N_ads = N_ads0*fraction + (1.0 - fraction)*N_ads1;
                        // float N_ads = m_values[H][pIndex];
                        numberOfAdsorbedAtoms[pIndex] += N_ads;
                    }
                    inside++;
                } else outside++;
            }
        }
    }

    float lx = 0;
    float ly = 0;
    float lz = 0;
    for(const float &dx : x) {
        lx += dx;
    }
    for(const float &dy : y) {
        ly += dy;
    }
    for(const float &dz : z) {
        lz += dz;
    }

    float cubicCentimetersPerLiter = 1000;
    float argonMass = 39.948;
    float avogadro = 6.0221409e+23;
    float argonDensity = 1.784;
    float argonLiterPerMol = argonMass / argonDensity;

    float xPlaneArea = ly*lz;
    float yPlaneArea = lx*lz;
    float zPlaneArea = lx*ly;
    float thickness  = 1.5;
    float xPlaneVolume = xPlaneArea*thickness;
    float yPlaneVolume = yPlaneArea*thickness;
    float zPlaneVolume = zPlaneArea*thickness;

    float totalZeoliteVolume = geometry->planesPerDimension()*(xPlaneVolume + yPlaneVolume + zPlaneVolume);
    float volumeOfZeoliteUnitCell = 5.21128;
    float massOfZeoliteUnitCell = 192*15.9994 + 96*28.0855;
    float numberOfZeoliteUnitCells = totalZeoliteVolume / volumeOfZeoliteUnitCell;
    float totalZeoliteMass = numberOfZeoliteUnitCells*massOfZeoliteUnitCell;
    float totalZeoliteMassGrams = totalZeoliteMass/avogadro;

    int numberOfUnitCellsBulkSystem = 200; // This is from MD.
    // float totalZeoliteMassBulkSystemGrams = numberOfUnitCellsBulkSystem*massOfZeoliteUnitCell/avogadro;
    float totalZeoliteVolumeBulkSystem = volumeOfZeoliteUnitCell*numberOfUnitCellsBulkSystem;
    float bulkSystemFactor = totalZeoliteVolume / totalZeoliteVolumeBulkSystem;

    m_points.clear();
    for(int i=0; i<m_pressures.size(); i++) {
        float N_adsorbed = numberOfAdsorbedAtoms[i];
        N_adsorbed += bulkSystemFactor*m_values[1][i];
        // N_adsorbed *= 1.41;
        float N_molesAdsorbed = N_adsorbed/avogadro;
        float volumeAdsorbedLiter = N_molesAdsorbed*argonLiterPerMol;
        float volumeAdsorbedCm3 = volumeAdsorbedLiter*cubicCentimetersPerLiter;
        float volumeAdsorbedCm3PerMassZeolite = volumeAdsorbedCm3 / totalZeoliteMassGrams;
        m_points.push_back(QPointF(m_pressures[i], volumeAdsorbedCm3PerMassZeolite));
    }
    setBins(m_points.size());
}

void Concentration::computeMode1(Geometry *geometry) {
    QVector<float> &x = geometry->deltaXVector();
    QVector<float> &y = geometry->deltaYVector();
    QVector<float> &z = geometry->deltaZVector();

    QVector<float> numberOfAdsorbedAtoms;
    numberOfAdsorbedAtoms.resize(m_pressures.size());
    for(float &v : numberOfAdsorbedAtoms) {
        v = 0.0;
    }

    int outside = 0;
    int inside = 0;
    double totalArea = 0.0;
    for(int i=0; i<x.size(); i++) {
        float deltas[3];
        deltas[0] = x[i];
        deltas[1] = y[i];
        deltas[2] = z[i];
        for(int a=0; a<3; a++) {
            float poreSize = deltas[a];
            const float poreVolume = poreSize*poreSize*poreSize;
            const float poreArea = 6*poreSize*poreSize;

            int H = round(poreSize);

            if(H>19) {
                H = 19; // force this
            }

            if(H>=2 && H<=19) {
                for(int pIndex=0; pIndex<m_pressures.size(); pIndex++) {
                    float N_ads = m_values[H][pIndex]/m_volumes[H]*poreVolume;
                    numberOfAdsorbedAtoms[pIndex] += N_ads;
                }
                totalArea += poreArea;
                inside++;
            } else outside++;
        }
    }

    // qDebug() << "Got Nads = " << numberOfAdsorbedAtoms << " and "

    float cubicCentimetersPerLiter = 1000;
    float argonMass = 39.948;
    float avogadro = 6.0221409e+23;
    float argonDensity = 1.784;
    float argonLiterPerMol = argonMass / argonDensity;

    float thickness  = 1.5;
    float totalZeoliteVolume = totalArea*thickness;
    float volumeOfZeoliteUnitCell = 5.21128;
    float massOfZeoliteUnitCell = 192*15.9994 + 96*28.0855;
    float numberOfZeoliteUnitCells = totalZeoliteVolume / volumeOfZeoliteUnitCell;
    float totalZeoliteMass = numberOfZeoliteUnitCells*massOfZeoliteUnitCell;
    float totalZeoliteMassGrams = totalZeoliteMass/avogadro;

    int numberOfUnitCellsBulkSystem = 200; // This is from MD.
    // float totalZeoliteMassBulkSystemGrams = numberOfUnitCellsBulkSystem*massOfZeoliteUnitCell/avogadro;
    float totalZeoliteVolumeBulkSystem = volumeOfZeoliteUnitCell*numberOfUnitCellsBulkSystem;
    float bulkSystemFactor = totalZeoliteVolume / totalZeoliteVolumeBulkSystem;

    m_points.clear();
    for(int i=0; i<m_pressures.size(); i++) {
        float N_adsorbed = numberOfAdsorbedAtoms[i];
        N_adsorbed += bulkSystemFactor*m_values[1][i];
        // N_adsorbed *= 1.41;
        float N_molesAdsorbed = N_adsorbed/avogadro;
        float volumeAdsorbedLiter = N_molesAdsorbed*argonLiterPerMol;
        float volumeAdsorbedCm3 = volumeAdsorbedLiter*cubicCentimetersPerLiter;
        float volumeAdsorbedCm3PerMassZeolite = volumeAdsorbedCm3 / totalZeoliteMassGrams;
        m_points.push_back(QPointF(m_pressures[i], volumeAdsorbedCm3PerMassZeolite));
    }
    setBins(m_points.size());
}

void Concentration::compute(Geometry *geometry)
{
    if(m_mode==0) computeMode0(geometry);
    if(m_mode==1) computeMode1(geometry);
}

QString Concentration::fileName() const
{
    return m_fileName;
}

void Concentration::setFileName(QString fileName)
{
    if (m_fileName == fileName)
        return;

    m_fileName = fileName;
    emit fileNameChanged(fileName);
    if(!m_fileName.isEmpty()) {
        readFile();
    }
}

bool Concentration::isValid()
{
    return m_pressures.size() > 0; // hack, but works.
}
