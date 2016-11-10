#include "concentration.h"
#include <QFile>
#include <QDebug>
#include <cmath>
#include <QTextStream>
#include <random.h>
#include "../montecarlo.h"
#include "../inifile.h"
#include "../geometry.h"
Concentration::Concentration(QObject *parent) : Statistic(parent)
{
    setConstant(false);
}

void Concentration::readFile(QString fileName) {
    QFile file(fileName);
    if(!file.open(QFileDevice::ReadOnly | QFileDevice::Text)) {
        qDebug() << "Could not find adsorption matrix file " << fileName;
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
            // qDebug() << "Read H = " << H << "  P = " << P << "  N = " << N << "  V = " << V;
        }
    }

    m_xLabel = "Pressure [p/p0]";
    m_yLabel = "V_ads/cm^3";
    setIsValid(true);
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
                // float poreSize = std::min(std::min(dx, dy), dz);
                const float poreVolume = dx*dy*dz;
                const float H = poreVolume;

                if(H>=1 && H<=19) {
                    for(int pIndex=0; pIndex<m_pressures.size(); pIndex++) {
                        double P = m_pressures[pIndex];
                        numberOfAdsorbedAtoms[pIndex] += findNumAdsorbed(P, dx, dy, dz);
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
    float xPlaneVolume = xPlaneArea*m_zeoliteThickness;
    float yPlaneVolume = yPlaneArea*m_zeoliteThickness;
    float zPlaneVolume = zPlaneArea*m_zeoliteThickness;

    float totalZeoliteVolume = (geometry->planesPerDimension() + 1)*(xPlaneVolume + yPlaneVolume + zPlaneVolume);
    float volumeOfZeoliteUnitCell = 5.21128;
    float massOfZeoliteUnitCell = 192*15.9994 + 96*28.0855;
    float numberOfZeoliteUnitCells = totalZeoliteVolume / volumeOfZeoliteUnitCell;
    float totalZeoliteMass = numberOfZeoliteUnitCells*massOfZeoliteUnitCell;
    float totalZeoliteMassGrams = totalZeoliteMass/avogadro;

    // int numberOfUnitCellsBulkSystem = 200; // This is from MD.
    // float totalZeoliteMassBulkSystemGrams = numberOfUnitCellsBulkSystem*massOfZeoliteUnitCell/avogadro;
    // float totalZeoliteVolumeBulkSystem = volumeOfZeoliteUnitCell*numberOfUnitCellsBulkSystem;
    // float bulkSystemFactor = totalZeoliteVolume / totalZeoliteVolumeBulkSystem;

    m_points.clear();
    for(int i=0; i<m_pressures.size(); i++) {
        float N_adsorbed = numberOfAdsorbedAtoms[i];
        //N_adsorbed += bulkSystemFactor*m_values[1][i];
        double adsorbedInZeolite = m_values[1][i]/m_volumes[1]*totalZeoliteVolume;
        N_adsorbed += adsorbedInZeolite;
        N_adsorbed *= m_scalingFactor;
        float N_molesAdsorbed = N_adsorbed/avogadro;
        float volumeAdsorbedLiter = N_molesAdsorbed*argonLiterPerMol;
        float volumeAdsorbedCm3 = volumeAdsorbedLiter*cubicCentimetersPerLiter;
        float volumeAdsorbedCm3PerMassZeolite = volumeAdsorbedCm3 / totalZeoliteMassGrams;
        m_points.push_back(QPointF(m_pressures[i], volumeAdsorbedCm3PerMassZeolite));
    }
    setBins(m_points.size());
    setMin(0);
    setMax(1.0);
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

    float cubicCentimetersPerLiter = 1000;
    float argonMass = 39.948;
    float avogadro = 6.0221409e+23;
    float argonDensity = 1.784;
    float argonLiterPerMol = argonMass / argonDensity;

    // float totalZeoliteVolume = totalArea*m_zeoliteThickness;
    float totalZeoliteVolume = totalArea*m_zeoliteThickness;

    for(int pIndex=0; pIndex<m_pressures.size(); pIndex++) {
        float poreVolume = totalZeoliteVolume;
        float N_ads = m_values[1][pIndex]/m_volumes[1]*poreVolume;
        numberOfAdsorbedAtoms[pIndex] += N_ads;
    }

    float volumeOfZeoliteUnitCell = 5.21128;
    float massOfZeoliteUnitCell = 192*15.9994 + 96*28.0855;
    float numberOfZeoliteUnitCells = totalZeoliteVolume / volumeOfZeoliteUnitCell;
    float totalZeoliteMass = numberOfZeoliteUnitCells*massOfZeoliteUnitCell;
    float totalZeoliteMassGrams = totalZeoliteMass/avogadro;

    // int numberOfUnitCellsBulkSystem = 200; // This is from MD.
    // float totalZeoliteMassBulkSystemGrams = numberOfUnitCellsBulkSystem*massOfZeoliteUnitCell/avogadro;
    // float totalZeoliteVolumeBulkSystem = volumeOfZeoliteUnitCell*numberOfUnitCellsBulkSystem;
    // float bulkSystemFactor = totalZeoliteVolume / totalZeoliteVolumeBulkSystem;

    m_points.clear();
    for(int i=0; i<m_pressures.size(); i++) {
        float N_adsorbed = numberOfAdsorbedAtoms[i];
        // N_adsorbed += bulkSystemFactor*m_values[1][i];
        N_adsorbed *= m_scalingFactor;
        float N_molesAdsorbed = N_adsorbed/avogadro;
        float volumeAdsorbedLiter = N_molesAdsorbed*argonLiterPerMol;
        float volumeAdsorbedCm3 = volumeAdsorbedLiter*cubicCentimetersPerLiter;
        float volumeAdsorbedCm3PerMassZeolite = volumeAdsorbedCm3 / totalZeoliteMassGrams;
        m_points.push_back(QPointF(m_pressures[i], volumeAdsorbedCm3PerMassZeolite));
    }
    setBins(m_points.size());
}

void Concentration::compute(Geometry *geometry, int timestep)
{
    if(!geometry || m_lastComputed == timestep) return;
    if(m_mode==0) computeMode0(geometry);
    if(m_mode==1) computeMode1(geometry);
    m_lastComputed = timestep;
}

double Concentration::zeoliteThickness() const
{
    return m_zeoliteThickness;
}

double Concentration::scalingFactor() const
{
    return m_scalingFactor;
}

bool Concentration::isValid()
{
    return m_pressures.size() > 0; // hack, but works.
}


void Concentration::loadIniFile(IniFile *iniFile)
{
    qDebug() << "Concentration starting to load ini file...";
    // First set source key to blank so parent won't attempt loading the file
    QString sourceKey = m_sourceKey;
    setSourceKey("");
    Statistic::loadIniFile(iniFile);
    setSourceKey(sourceKey);

    if(m_sourceKey.isEmpty()) return;
    QString fileName = iniFile->getString(m_sourceKey);
    readFile(fileName);
    m_points.clear(); // Clear what we read in base class

    qDebug() << "Concentration loaded ini file with ";
    qDebug() << "  Source key: " << m_sourceKey;
    qDebug() << "  Filename: " << fileName;
    setIsValid(true);
}

void Concentration::setZeoliteThickness(double zeoliteThickness)
{
    if (m_zeoliteThickness == zeoliteThickness)
        return;

    m_zeoliteThickness = zeoliteThickness;
    emit zeoliteThicknessChanged(zeoliteThickness);
}

void Concentration::setScalingFactor(double scalingFactor)
{
    if (m_scalingFactor == scalingFactor)
        return;

    m_scalingFactor = scalingFactor;
    emit scalingFactorChanged(scalingFactor);
}


void Concentration::saveState(QFile &file)
{

}


double Concentration::findNumAdsorbed(double P, double Lx, double Ly, double Lz) {
    double eps = 1e-7;
    if(fabs(P-0.0001) < eps) {
        double a = 0.07239;
        double b = -0.4811;
        double c = 7.335e-10;
        double V = Lx*Ly*Lz;
        double Nads = (a*pow(V, b) + c);
        return Nads*V;
    } else if(fabs(P-0.0005) < eps) {
        double a = 0.3398;
        double b = -0.4942;
        double c = 3.897e-12;
        double V = Lx*Ly*Lz;
        double Nads = (a*pow(V, b) + c);
        return Nads*V;
    } else if(fabs(P-0.001) < eps) {
        double a = 0.6653;
        double b = -0.5059;
        double c = 2.221e-9;
        double V = Lx*Ly*Lz;
        double Nads = (a*pow(V, b) + c);
        return Nads*V;
    } else if(fabs(P-0.059765) < eps) {
        double a = 19.64;
        double b = -0.4823;
        double c = 2.725e-8;
        double V = Lx*Ly*Lz;
        double Nads = (a*pow(V, b) + c);
        return Nads*V;
    } else if(fabs(P-0.118529) < eps) {
        double a = 17.34;
        double b = -0.432;
        double c = 6.69e-9;
        double V = Lx*Ly*Lz;
        double Nads = (a*pow(V, b) + c);
        if(V<7) {
            Nads = 17;
        }
        return Nads*V;
    } else if(fabs(P-0.177294) < eps) {
        double a = 49.54;
        double b = -0.6381;
        double c = 0.3861;
        double V = Lx*Ly*Lz;
        double Nads = (a*pow(V, b) + c);
        if(V < 24) {
            Nads = -0.4865*V + 18.21;
        }
        return Nads*V;
    } else if(fabs(P-0.236059) < eps) {
        double a = 59.79;
        double b = -0.5839;
        double c = 0.4853;
        double V = Lx*Ly*Lz;
        double Nads = (a*pow(V, b) + c);
        return Nads*V;
    } else if(fabs(P-0.294824) < eps) {
        double a = 44.05;
        double b = -0.4952;
        double c = 0.3104;
        double V = Lx*Ly*Lz;
        double Nads = (a*pow(V, b) + c);
        if(V < 24) {
            Nads = 18;
        }
        return Nads*V;
    } else if(fabs(P-0.353588) < eps) {
        double a = 53.02;
        double b = -0.4994;
        double c = 0.4253;
        double V = Lx*Ly*Lz;
        double Nads = (a*pow(V, b) + c);
        if(V < 64) {
            Nads = -0.1892*V + 18.42;
        }
        return Nads*V;
    } else if(fabs(P-0.412353) < eps) {
        double a = 105.5;
        double b = -0.606;
        double c = 0.8105;
        double V = Lx*Ly*Lz;
        double Nads = (a*pow(V, b) + c);
        if(V < 100) {
            Nads = -0.1219*V + 18.78;
        }
        return Nads*V;
    } else if(fabs(P-0.471118) < eps) {
        double a = 38.19;
        double b = -0.3737;
        double c = 0.1e-8;
        double V = Lx*Ly*Lz;
        double Nads = (a*pow(V, b) + c);
        if(V < 50) {
            Nads = 19;
        } else if(V<100) {
            Nads = -0.02937*V + 11.55;
        }
        return Nads*V;
    } else if(fabs(P-0.529882) < eps) {
        double a = 42.76;
        double b = -0.3734;
        double V = Lx*Ly*Lz;
        double Nads = a*pow(V, b);
        if(V < 50) {
            Nads = 19;
        } else if(V<200) {
            a = 100.5;
            b = -0.5052;
            Nads = a*pow(V, b);
        }
        return Nads*V;
    } else if(fabs(P-0.588647) < eps) {
        double a = 88.64;
        double b = -0.4856;
        double c = 0.8597;
        double V = Lx*Ly*Lz;
        double Nads = a*pow(V, b) + c;
        if(V < 64) {
            Nads = 19;
        }
        return Nads*V;
    } else if(fabs(P-0.647412) < eps) {
        double a = 86.01;
        double b = -0.4571;
        double c = 0.4921;
        double V = Lx*Ly*Lz;
        double Nads = a*pow(V, b) + c;
        if(V < 100) {
            Nads = 19;
        }
        return Nads*V;
    } else if(fabs(P-0.706176) < eps) {
        double a = 88.31;
        double b = -0.4229;
        double c = 0.0;
        double V = Lx*Ly*Lz;
        double Nads = a*pow(V, b) + c;
        if(V < 100) {
            Nads = 19;
        } else if(V < 200) {
            Nads = -0.05365*V + 20.39;
        }
        return Nads*V;
    } else if(fabs(P-0.764941) < eps) {
        double a = 97.8;
        double b = -0.4221;
        double c = 0.0;
        double V = Lx*Ly*Lz;
        double Nads = a*pow(V, b) + c;
        if(V < 200) {
            Nads = 19;
        }
        return Nads*V;
    } else if(fabs(P-0.823706) < eps) {
        double a = 99.98;
        double b = -0.4078;
        double c = 0.0;
        double V = Lx*Ly*Lz;
        double Nads = a*pow(V, b) + c;
        if(V < 200) {
            Nads = 19.5;
        }
        return Nads*V;
    } else if(fabs(P-0.882471) < eps) {
        double a = 186.4;
        double b = -0.4609;
        double c = 0.0;
        double V = Lx*Ly*Lz;
        double Nads = a*pow(V, b) + c;
        if(V < 400) {
            Nads = 20.0;
        }
        return Nads*V;
    } else if(fabs(P-0.94123) < eps) {
        double a = 595.3;
        double b = -0.4609;
        double c = 2.463e-6;
        double V = Lx*Ly*Lz;
        double Nads = a*pow(V, b) + c;
        if(V < 400) {
            Nads = 20.0;
        }
        return Nads*V;
    } else {
        qDebug() << "Error, pressure not found. Can't compute concentration";
        exit(1);
    }
}
