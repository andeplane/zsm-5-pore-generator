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
                if(m_adsorption) {
                    // Use fitted model accounting for asymmetric pore volumes
                    for(int pIndex=0; pIndex<m_pressures.size(); pIndex++) {
                        double P = m_pressures[pIndex];
                        numberOfAdsorbedAtoms[pIndex] += findNumAdsorbed(P, dx, dy, dz);
                    }
                } else {
                    float poreSize = std::min(std::min(dx, dy), dz);
                    const float poreVolume = dx*dy*dz;
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

bool Concentration::adsorption() const
{
    return m_adsorption;
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

void Concentration::setAdsorption(bool adsorption)
{
    if (m_adsorption == adsorption)
        return;

    m_adsorption = adsorption;
    emit adsorptionChanged(adsorption);
}


void Concentration::saveState(QFile &file)
{

}


double Concentration::findNumAdsorbed(double P, double Lx, double Ly, double Lz) {
    double V = Lx*Ly*Lz;
    double eps = 1e-7;
    // Sort the three values
    double H0 = Lx;
    double H1 = Ly;
    double H2 = Lz;
    if(H1<H0) std::swap(H1, H0);
    if(H2<H0) std::swap(H2, H0);
    if(H2<H1) std::swap(H2, H1);

    if(fabs(P-0.0001) < eps) {
        double a = 0.04307;
        double b = -0.5746;
        double c = 0;
        double Nads = (a*pow(V, b) + c);
        return Nads*V;
    } else if(fabs(P-0.0005) < eps) {
        double H_table[] = {1.0, 1.03, 1.03};
        double V_table = 1.07;
        double Nads = 0.25;
        if( (H0>H_table[0] || H1>H_table[1] || H2>H_table[2]) && V>V_table) {
            double a = 0.255;
            double b = -0.556;
            double c = 0.0;
            Nads = (a*pow(V, b) + c);
        }
        return Nads*V;
    } else if(fabs(P-0.001) < eps) {
        double H_table[] = {1.0, 2, 2};
        double V_table = 4;
        double Nads = 0.41;
        if( (H0>H_table[0] || H1>H_table[1] || H2>H_table[2]) && V>V_table) {
            double a = 2.59;
            double b = -1.36;
            double c = 0.01386;
            Nads = (a*pow(V, b) + c);
        }
        return Nads*V;
    } else if(fabs(P-0.059765) < eps) {
        double H_table[] = {1.0, 1.5, 1.5};
        double V_table = 2.25;
        double Nads = 16.36;
        if( (H0>H_table[0] || H1>H_table[1] || H2>H_table[2]) && V>V_table) {
            double a = 28.52;
            double b = -0.6881;
            double c = 0.04328;
            Nads = (a*pow(V, b) + c);
        }
        return Nads*V;
    } else if(fabs(P-0.118529) < eps) {
        double H_table[] = {1.0, 3, 3};
        double V_table = 9;
        double Nads = 17.54;
        if( (H0>H_table[0] || H1>H_table[1] || H2>H_table[2]) && V>V_table) {
            double a = 169.9;
            double b = -1.046;
            double c = 0.4814;
            Nads = (a*pow(V, b) + c);
        }
        return Nads*V;
    } else if(fabs(P-0.177294) < eps) {
        double H_table[] = {1.85, 1.85, 1.85};
        double V_table = 6.3;
        double Nads = 18.15;
        if( (H0>H_table[0] || H1>H_table[1] || H2>H_table[2]) && V>V_table) {
            double a = 66.9;
            double b = -0.7244;
            double c = 0.5385;
            Nads = (a*pow(V, b) + c);
        }
        return Nads*V;
    } else if(fabs(P-0.236059) < eps) {
        double H_table[] = {2, 2, 2};
        double V_table = 8;
        double Nads = 18.48;
        if( (H0>H_table[0] || H1>H_table[1] || H2>H_table[2]) && V>V_table) {
            double a = 61.56;
            double b = -0.5896;
            double c = 0.4853;
            Nads = (a*pow(V, b) + c);
        }
        return Nads*V;
    } else if(fabs(P-0.294824) < eps) {
        double H_table[] = {2, 2.45, 2.45};
        double V_table = 12;
        double Nads = 17.7;
        if( (H0>H_table[0] || H1>H_table[1] || H2>H_table[2]) && V>V_table) {
            double a = 99.31;
            double b = -0.71663;
            double c = 0.9433;
            Nads = (a*pow(V, b) + c);
        }
        return Nads*V;
    } else if(fabs(P-0.353588) < eps) {
        double H_table[] = {2, 3, 3};
        double V_table = 18;
        double Nads = 17.53;
        if( (H0>H_table[0] || H1>H_table[1] || H2>H_table[2]) && V>V_table) {
            double a = 125.8;
            double b = -0.703;
            double c = 1.044;
            Nads = (a*pow(V, b) + c);
        }
        return Nads*V;
    } else if(fabs(P-0.412353) < eps) {
        double H_table[] = {2, 3.1, 3.1};
        double V_table = 18.8;
        double Nads = 18.8; //TODO: equal vtable and nads coincindence?
        if( (H0>H_table[0] || H1>H_table[1] || H2>H_table[2]) && V>V_table) {
            double a = 115;
            double b = -0.6356;
            double c = 0.9894;
            Nads = (a*pow(V, b) + c);
        }

        return Nads*V;
    } else if(fabs(P-0.471118) < eps) {
        double H_table[] = {3,3,4};
        double V_table = 36;
        double Nads = 17.8;
        if( (H0>H_table[0] || H1>H_table[1] || H2>H_table[2]) && V>V_table) {
            double a = 450.1;
            double b = -0.9366;
            double c = 2.089;
            Nads = (a*pow(V, b) + c);
        }

        return Nads*V;
    } else if(fabs(P-0.529882) < eps) {
        double H_table[] = {3, 3.8, 3.8};
        double V_table = 44;
        double Nads = 18.72;
        if( (H0>H_table[0] || H1>H_table[1] || H2>H_table[2]) && V>V_table) {
            double a = 948.4;
            double b = -1.079;
            double c = 2.738; // Missing in the formula?
            double V = Lx*Ly*Lz;
            Nads = a*pow(V, b) + c;
        }
        return Nads*V;
    } else if(fabs(P-0.588647) < eps) {
        double H_table[] = {3, 4, 4};
        double V_table = 48;
        double Nads = 18.7;
        if( (H0>H_table[0] || H1>H_table[1] || H2>H_table[2]) && V>V_table) {
            double a = 270.3;
            double b = -0.7111;
            double c = 1.485;
            Nads = a*pow(V, b) + c;
        }

        return Nads*V;
    } else if(fabs(P-0.647412) < eps) {
        double H_table[] = {3, 5, 5};
        double V_table = 75;
        double Nads = 18.9;
        if( (H0>H_table[0] || H1>H_table[1] || H2>H_table[2]) && V>V_table) {
            double a = 1244;
            double b = -1.008;
            double c = 2.884;
            Nads = a*pow(V, b) + c;
        }

        return Nads*V;
    } else if(fabs(P-0.706176) < eps) {
        double H_table[] = {4, 5, 5};
        double V_table = 100;
        double Nads = 18.8;
        if( (H0>H_table[0] || H1>H_table[1] || H2>H_table[2]) && V>V_table) {
            double a = 4758;
            double b = -1.256;
            double c = 4.051;
            Nads = a*pow(V, b) + c;
        }

        return Nads*V;
    } else if(fabs(P-0.764941) < eps) {
        double H_table[] = {4, 5.7, 5.7};
        double V_table = 130;
        double Nads = 19.8;
        if( (H0>H_table[0] || H1>H_table[1] || H2>H_table[2]) && V>V_table) {
            double a = 5918;
            double b = -1.212;
            double c = 3.577;
            Nads = a*pow(V, b) + c;
        }
        return Nads*V;
    } else if(fabs(P-0.823706) < eps) {
        double V_table = 230;
        double Nads = 20.3;
        if(V>V_table) {
            double a = 180200;
            double b = -1.715;
            double c = 4.23;
            Nads = a*pow(V, b) + c;
        }

        return Nads*V;
    } else if(fabs(P-0.882471) < eps) {
        double H_table[] = {6, 7.75, 7.75};
        double V_table = 360;
        double Nads = 20.3;
        if( (H0>H_table[0] || H1>H_table[1] || H2>H_table[2]) && V>V_table) {
            double a = 1.58e8;
            double b = -2.748;
            double c = 5.328;
            Nads = a*pow(V, b) + c;
        }

        return Nads*V;
    } else if(fabs(P-0.941235) < eps) {
        double H_table[] = {6, 9, 9};
        double V_table = 486;
        double Nads = 20.0;
        if( (H0>H_table[0] || H1>H_table[1] || H2>H_table[2]) && V>V_table) {
            double a = 172600;
            double b = -1.494;
            double c = 3.352;
            Nads = a*pow(V, b) + c;
        }

        return Nads*V;
    } else {
        qDebug() << "Error, pressure " << P << " not found. Can't compute concentration";
        exit(1);
    }
}
