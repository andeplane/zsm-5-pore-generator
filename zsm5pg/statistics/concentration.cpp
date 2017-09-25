#include "concentration.h"
#include <QFile>
#include <omp.h>
#include <QDebug>
#include <cmath>
#include <QTextStream>
#include <random.h>
#include "../montecarlo.h"
#include "../inifile.h"
#include "../geometry.h"
#include <vector>
using namespace std;

Concentration::Concentration(QObject *parent) : Statistic(parent)
{
    setConstant(false);
}

void Concentration::readFileOct16(QString fileName) {
    qDebug() << "Concentration loading file " << fileName;

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

void Concentration::readJan17Files() {
    for(QString P : m_pressureStrings) {
        float PValue = P.toFloat();
        m_pressures.push_back(PValue);
    }
    reader.debug = false;
    reader.folder = "/projects/zeolite/3dmodel/interpolatedData/data";
    reader.largestPoreSize = 18;
    qDebug() << "Reading GCMC data...";
    reader.readData();
    qDebug() << "Done.";
    m_xLabel = "Pressure [p/p0]";
    m_yLabel = "V_ads/cm^3";
    setIsValid(true);
}

void Concentration::computeMode0(Geometry *geometry) {
    if(!geometry) return;

    QVector<float> numberOfAdsorbedAtoms;
    numberOfAdsorbedAtoms.resize(m_pressures.size());
    for(float &v : numberOfAdsorbedAtoms) {
        v = 0.0;
    }

    int outside = 0;
    int inside = 0;
    QString dataMode = m_adsorption ? "adsorption" : "desorption";
#define DOOMP
#ifdef DOOMP
#pragma omp parallel num_threads(10)
{
#endif
    const QVector<float> xx = geometry->deltaXVector();
    const QVector<float> yy = geometry->deltaYVector();
    const QVector<float> zz = geometry->deltaZVector();
    const QVector<float> pressures = m_pressures;
#ifdef DOOMP
#pragma omp for
#endif
    for(int pIndex=0; pIndex<pressures.size(); pIndex++) {
        double sum = 0;
        for(int i=0; i<xx.size(); i++) {
            const float dx = xx[i];
            for(int j=0; j<yy.size(); j++) {
                const float dy = yy[j];
                for(int k=0; k<zz.size(); k++) {
                    const float dz = zz[k];
                    double H0 = dx;
                    double H1 = dy;
                    double H2 = dz;

                    if(H1<H0) std::swap(H1, H0);
                    if(H2<H0) std::swap(H2, H0);
                    if(H2<H1) std::swap(H2, H1);

                    QString P = m_pressureStrings[pIndex];

                    if(m_adsorption)
                        sum += reader.getNum(reader.CAds[P], reader.NAds[P], H0, H1, H2);
                    else
                        sum += reader.getNum(reader.CDes[P], reader.NDes[P], H0, H1, H2);
                    //sum += findNumAdsorbedJan17(dataMode, P, dx, dy, dz);
                }
            }
        }
        numberOfAdsorbedAtoms[pIndex] += sum;
    }
#ifdef DOOMP
}
#endif
    const QVector<float> &x = geometry->deltaXVector();
    const QVector<float> &y = geometry->deltaYVector();
    const QVector<float> &z = geometry->deltaZVector();
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
        // double adsorbedInZeolite = m_values[1][i]/m_volumes[1]*totalZeoliteVolume; // 16 data
        double adsorbedInZeolite = 0;
        if(m_adsorption) {
            adsorbedInZeolite = reader.CAdsWall[m_pressureStrings[i]] * totalZeoliteVolume;
        } else {
            adsorbedInZeolite = reader.CDesWall[m_pressureStrings[i]] * totalZeoliteVolume;
        }

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
    qDebug() << "Will return " << (m_pressures.size()>0);
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
    // readFile(fileName);
    // readFileJan17(fileName);
    readJan17Files();

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

double Concentration::findNumAdsorbedOct16(double P, double Lx, double Ly, double Lz) {
    double V = Lx*Ly*Lz;
    double eps = 1e-7;
    // Sort the three values
    double H0 = Lx;
    double H1 = Ly;
    double H2 = Lz;
    if(H1<H0) std::swap(H1, H0);
    if(H2<H0) std::swap(H2, H0);
    if(H2<H1) std::swap(H2, H1);
    H0 = round(H0);
    H1 = round(H1);
    H2 = round(H2);

    if(fabs(P-0.0001) < eps) {
        double a = 0.06441;
        double b = -0.8238;
        double c = 0.0003653;
        double Nads = (a*pow(V, b) + c);
        return Nads*V;
    } else if(fabs(P-0.0005) < eps) {
        double a = 0.5005;
        double b = -0.9803;
        double c = 0.004008;
        double Nads = (a*pow(V, b) + c);
        return Nads*V;
    } else if(fabs(P-0.001) < eps) {
        double a = 1.744;
        double b = -1.197;
        double c = 0.01088;
        double Nads = (a*pow(V, b) + c);
        return Nads*V;
    } else if(fabs(P-0.059765) < eps) {
        double a = 12.22;
        double b = -0.4923;
        double c = 0.07557;
        double Nads = (a*pow(V, b) + c);
        return Nads*V;
    } else if(fabs(P-0.118529) < eps) {
        double a = 22.18;
        double b = -0.4987;
        double c = 0.3375;
        double Nads = (a*pow(V, b) + c);
        return Nads*V;
    } else if(fabs(P-0.177294) < eps) {
            double a = 62.76;
            double b = -0.7297;
            double c = 1.379;
            double Nads = (a*pow(V, b) + c);
        return Nads*V;
    } else if(fabs(P-0.236059) < eps) {
        if(m_adsorption) {
            double a = 56.87;
            double b = -0.5598;
            double c = 0.7598;
            double Nads = (a*pow(V, b) + c);
            return Nads*V;
        }
        // Desorption
        vector<double> H_table = {2, 3, 3};
        double Nads = 17.7;
        double V_table = 18;
        if( !(H0<H_table[0] || H1<H_table[1] || H2<H_table[2] || V<V_table)) {
            double a = 1121;
            double b = -1.494;
            double c = 2.771;
            Nads = (a*pow(V, b) + c);
        }
        return Nads*V;
    } else if(fabs(P-0.294824) < eps) {
        if(m_adsorption) {
            double H_table[] = {2, 2.6, 2.6};
            double V_table = 13.2;
            double Nads = 18.2;
            if( !(H0<H_table[0] || H1<H_table[1] || H2<H_table[2] || V<V_table)) {
                double a = 99.31;
                double b = -0.7;
                double c = 1.918;
                Nads = (a*pow(V, b) + c);
            }
            return Nads*V;
        }

        vector<double> H_table = {2, 5, 5};
        if(H0==2) H_table = {2, 5, 5};
        else if(H0==3) H_table = {3,3.9,3.9};

        double V_table = 45;
        double Nads = 18;

        if( !(H0<H_table[0] || H1<H_table[1] || H2<H_table[2] || V<V_table)) {
            double a = 601.4;
            double b = -0.9431;
            double c = 1.474;
            Nads = (a*pow(V, b) + c);
        }
        return Nads*V;
    } else if(fabs(P-0.353588) < eps) {
        if(m_adsorption) {
            double H_table[] = {2, 3, 3};
            double V_table = 15;
            double Nads = 18.5;
            if( !(H0<H_table[0] || H1<H_table[1] || H2<H_table[2] || V<V_table)) {
                double a = 140;
                double b = -0.8147;
                double c = 3.067;
                Nads = (a*pow(V, b) + c);
            }
            return Nads*V;
        }
        // Desorption
        vector<double> H_table = {3, 5, 5};
        if(H0==3) H_table = {3, 5, 5};
        else if(H0==4) H_table = {4, 4.3, 4.3};

        double V_table = 80;
        double Nads = 18.2;

        if( !(H0<H_table[0] || H1<H_table[1] || H2<H_table[2] || V<V_table)) {
            double a = 13060;
            double b = -1.5434;
            double c = 2.483;
            Nads = (a*pow(V, b) + c);
        }
        return Nads*V;
    } else if(fabs(P-0.412353) < eps) {
        if(m_adsorption) {
            double H_table[] = {2, 3.1, 3.1};
            double V_table = 19.0;
            double Nads = 18.8;
            if( !(H0<H_table[0] || H1<H_table[1] || H2<H_table[2] || V<V_table)) {
                double a = 157;
                double b = -0.7851;
                double c = 3.257;
                Nads = (a*pow(V, b) + c);
            }
            return Nads*V;
        }

        // Desorption
        vector<double> H_table = {3, 5.5, 5.5};
        if(H0==3) H_table = {3, 5.5, 5.5};
        else if(H0==4) H_table = {4, 5, 5};
        double V_table = 90;
        double Nads = 18.8;
        if( !(H0<H_table[0] || H1<H_table[1] || H2<H_table[2] || V<V_table)) {
            double a = 5.411e5;
            double b = -2.31;
            double c = 3.443;
            Nads = (a*pow(V, b) + c);
        }
        return Nads*V;
    } else if(fabs(P-0.471118) < eps) {
        if(m_adsorption) {
            vector<double> H_table = {3,3,3.5};
            if(H0==2) H_table = {2,6,6};
            double Nads = 19.0;
            double V_table = 31.5;
            if( !(H0<H_table[0] || H1<H_table[1] || H2<H_table[2] || V<V_table)) {
                double a = 1025;
                double b = -1.238;
                double c = 4.678;
                Nads = (a*pow(V, b) + c);
            }
            return Nads*V;
        }

        // Desorption
        vector<double> H_table = {3, 5.5, 5.5};
        if(H0==3) H_table = {3, 5.5, 5.5};
        else if(H0==4 && H1>=5) H_table = {4, 5, 5};
        else if(H0==4) H_table = {4, 4, 8};
        double Nads = 18.8;
        double V_table = 90;
        if( !(H0<H_table[0] || H1<H_table[1] || H2<H_table[2] || V<V_table)) {
            double a = 3.142e5;
            double b = -2.198;
            double c = 3.94;
            Nads = (a*pow(V, b) + c);
        }
        return Nads*V;

    } else if(fabs(P-0.529882) < eps) {
        if(m_adsorption) {
            vector<double> H_table = {3,3.8,3.8};
            if(H0==2) H_table = {2,7,7};
            double Nads = 18.4;
            double V_table = 44;
            if( !(H0<H_table[0] || H1<H_table[1] || H2<H_table[2] || V<V_table)) {
                double a = 1421;
                double b = -1.239;
                double c = 5.318;
                Nads = (a*pow(V, b) + c);
            }
            return Nads*V;
        }

        // Desorption
        vector<double> H_table = {4,5,6};
        if(H1<5) H_table = {4,4,8};
        double Nads = 20.8;
        double V_table = 90;
        if( !(H0<H_table[0] || H1<H_table[1] || H2<H_table[2] || V<V_table)) {
            double a = 1.572e5;
            double b = -2.037;
            double c = 4.403;
            Nads = (a*pow(V, b) + c);
        }
        return Nads*V;
    } else if(fabs(P-0.588647) < eps) {
        if(m_adsorption) {
            vector<double> H_table = {3,4,4};
            double Nads = 18.4;
            double V_table = 48;
            if( !(H0<H_table[0] || H1<H_table[1] || H2<H_table[2] || V<V_table)) {
                double a = 1600;
                double b = -1.271;
                double c = 6.695;
                Nads = (a*pow(V, b) + c);
            }
            return Nads*V;
        }

        // Desorption
        vector<double> H_table = {4,5,6};
        if(H1<5) H_table = {4,4,8};
        double Nads = 20.8;
        double V_table = 120;
        if( !(H0<H_table[0] || H1<H_table[1] || H2<H_table[2] || V<V_table)) {
            double a = 4.624e15;
            double b = -6.994;
            double c = 6.735;
            Nads = (a*pow(V, b) + c);
        }
        return Nads*V;
    } else if(fabs(P-0.647412) < eps) {
        if(m_adsorption) {
            vector<double> H_table = {4,4,4};
            if(H0<4) H_table = {3,5,5};
            double Nads = 19.1;
            double V_table = 75;
            if( !(H0<H_table[0] || H1<H_table[1] || H2<H_table[2] || V<V_table)) {
                double a = 1924;
                double b = -1.137;
                double c = 4.923;
                Nads = (a*pow(V, b) + c);
            }
            return Nads*V;
        }

        // Desorption
        vector<double> H_table = {4,6,6};
        double Nads = 20.0;
        double V_table = 75;
        if( !(H0<H_table[0] || H1<H_table[1] || H2<H_table[2] || V<V_table)) {
            double a = 1924;
            double b = -1.137;
            double c = 4.923;
            Nads = (a*pow(V, b) + c);
        }
        return Nads*V;

    } else if(fabs(P-0.706176) < eps) {
        if(m_adsorption) {
            vector<double> H_table = {4,5,5};
            if(H1<5) H_table = {4,4,10};
            double Nads = 18.8;
            double V_table = 100;
            if( !(H0<H_table[0] || H1<H_table[1] || H2<H_table[2] || V<V_table)) {
                double a = 10220;
                double b = -1.454;
                double c = 5.795;
                Nads = (a*pow(V, b) + c);
            }
            return Nads*V;
        }

        // Desorption
        vector<double> H_table = {4,6,6};
        double Nads = 20.0;
        double V_table = 100;
        if( !(H0<H_table[0] || H1<H_table[1] || H2<H_table[2] || V<V_table)) {
            double a = 10220;
            double b = -1.454;
            double c = 5.795;
            Nads = (a*pow(V, b) + c);
        }
        return Nads*V;
    } else if(fabs(P-0.764941) < eps) {
        if(m_adsorption) {
            vector<double> H_table = {4,5.7,5.7};
            if(H0<4) H_table = {3, 10, 10};
            double Nads = 20.3;
            double V_table = 130;
            if( !(H0<H_table[0] || H1<H_table[1] || H2<H_table[2] || V<V_table)) {
                double a = 20650;
                double b = -1.49;
                double c = 5.721;
                Nads = (a*pow(V, b) + c);
            }
            return Nads*V;
        }

        // Desorption
        vector<double> H_table = {4,7,7};
        double Nads = 20.3;
        double V_table = 130;
        if( !(H0<H_table[0] || H1<H_table[1] || H2<H_table[2] || V<V_table)) {
            double a = 20650;
            double b = -1.49;
            double c = 5.721;
            Nads = (a*pow(V, b) + c);
        }
        return Nads*V;
    } else if(fabs(P-0.823706) < eps) {
        if(m_adsorption) {
            vector<double> H_table = {4,12,12};
            if(H0<4) H_table = {3,15,15};
            double Nads = 20.3;
            double V_table = 230;
            if( !(H0<H_table[0] || H1<H_table[1] || H2<H_table[2] || V<V_table)) {
                double a = 24450;
                double b = -1.364;
                double c = 5.27;
                Nads = (a*pow(V, b) + c);
            }
            return Nads*V;
        }

        // Desorption
        vector<double> H_table = {4,12,12};
        double Nads = 20.3;
        double V_table = 230;
        if( !(H0<H_table[0] || H1<H_table[1] || H2<H_table[2] || V<V_table)) {
            double a = 24450;
            double b = -1.364;
            double c = 5.27;
            Nads = (a*pow(V, b) + c);
        }
        return Nads*V;
    } else if(fabs(P-0.882471) < eps) {
        vector<double> H_table = {4,12,15};
        double Nads = 20.3;
        double V_table = 720;
        if( !(H0<H_table[0] || H1<H_table[1] || H2<H_table[2] || V<V_table)) {
            double a = 7.346e15;
            double b = -5.176;
            double c = 8.014;
            Nads = (a*pow(V, b) + c);
        }
        return Nads*V;
    } else if(fabs(P-0.941235) < eps) {
        vector<double> H_table = {4,12,15};
        double Nads = 20.0;
        double V_table = 720;
        if( !(H0<H_table[0] || H1<H_table[1] || H2<H_table[2] || V<V_table)) {
            double a = 1.414e7;
            double b = -2.116;
            double c = 7.297;
            Nads = (a*pow(V, b) + c);
        }
        return Nads*V;
    } else {
        qDebug() << "Error, pressure " << P << " not found. Can't compute concentration";
        exit(1);
    }
}


double Concentration::findNumAdsorbedJan17(QString mode, QString P, double Lx, double Ly, double Lz) {
    // Sort the three values
    int H0 = round(Lx);
    int H1 = round(Ly);
    int H2 = round(Lz);

    if(H1<H0) std::swap(H1, H0);
    if(H2<H0) std::swap(H2, H0);
    if(H2<H1) std::swap(H2, H1);

    if(mode=="adsorption") {
        return reader.getNum(reader.CAds[P], reader.NAds[P], Lx, Ly, Lz);
    } else {
        return reader.getNum(reader.CDes[P], reader.NDes[P], Lx, Ly, Lz);
    }
}
