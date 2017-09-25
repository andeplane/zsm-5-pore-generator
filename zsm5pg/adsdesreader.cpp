#include "adsdesreader.h"
#include <QDebug>
#include <QFile>
#include <cmath>
AdsDesReader::AdsDesReader()
{
    oneOverDeltaPoreSize = -1;
    largestPoreSize = 18;
    resampleSize = 2000;
    debug = false;
    modes = QStringList({ QString("adsorption"), QString("desorption")});
}

void AdsDesReader::readH2H3Matrices() {
    QString fileNameH2 = folder+QString("/H2_adsorption_H2_P0.000100.txt");
    QString fileNameH3 = folder+QString("/H3_adsorption_H2_P0.000100.txt");
    QFile fileH2(fileNameH2);
    QFile fileH3(fileNameH3);
    if(!fileH2.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Could not open file " << fileH2.fileName();
        exit(0);
    }
    if(!fileH3.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Could not open file " << fileH3.fileName();
        exit(0);
    }
    QStringList linesH2 = QString(fileH2.readAll()).split("\n");
    QStringList linesH3 = QString(fileH3.readAll()).split("\n");
    int m = linesH2.length()-1;
    int n = linesH2[0].split(" ").length();
    if(debug) qDebug() << "Found " << m << " x " << n << " matrix";

    for(int i=0; i<m; i++) {
        QString word = linesH2[i].split(" ")[0]; // First number is our value
        bool ok;
        float value = word.toFloat(&ok);
        if(!ok) { qDebug() << "Warning, could not parse " << word; }
        H2.push_back(value);
    }

    QStringList words = linesH3[0].split(" ");
    for(int i=0; i<n; i++) {
        bool ok;
        float value = words[i].toFloat(&ok);
        if(!ok) { qDebug() << "Warning, could not parse " << words[i]; }
        H3.push_back(value);
    }

    deltaPoreSize = H2[1]-H2[0];
    oneOverDeltaPoreSize = 1.0/deltaPoreSize;

    for(QString P : pressures) {
        NAds[P] = QVector<QVector<QVector<float>>>();
        NDes[P] = QVector<QVector<QVector<float>>>();
        NAds[P].resize(20);
        NDes[P].resize(20);

        CAds[P] = QVector<QVector<QVector<float>>>();
        CDes[P] = QVector<QVector<QVector<float>>>();
        CAds[P].resize(20);
        CDes[P].resize(20);
        for(int i=0; i<20; i++) {
            NAds[P][i].resize(H2.size());
            NDes[P][i].resize(H2.size());
            CAds[P][i].resize(H2.size());
            CDes[P][i].resize(H2.size());
        }
    }

    H2.push_back(H2.last()+deltaPoreSize);
    H3.push_back(H3.last()+deltaPoreSize);
}

void AdsDesReader::readData()
{
    if(debug) qDebug() << "Reading H2H3 matrices";
    readH2H3Matrices();
    if(debug) qDebug() << "Reading concentration values";
    for(QString mode : modes) {
        for(QString P : pressures) {
            for(int H1=2; H1<=largestPoreSize; H1++) {
                readFile(H1, P, mode);
            }
        }
        readWall(mode);
    }
    if(debug) qDebug() << "Finished loading all data.";
    appendDummyData();
}

void AdsDesReader::appendDummyData()
{
    for(QString P : pressures) {
        for(int H1=2; H1<=largestPoreSize; H1++) {
            // Append a full extra index for H2 and H3 so that interpolation
            // is ok even for H2=19 or H3=19

            for(int i=0; i<NAds[P][H1].size(); i++) {
                if(NAds[P][H1][i].size() > 0) { // Only append if we have any values here
                    NAds[P][H1][i].push_back(NAds[P][H1][i].last());
                    NDes[P][H1][i].push_back(NDes[P][H1][i].last());
                    CAds[P][H1][i].push_back(CAds[P][H1][i].last());
                    CDes[P][H1][i].push_back(CDes[P][H1][i].last());
                }
            }

            // Append last value for H3>19
            for(int i=NAds[P][H1].size()-2; i>=0; i--) {
                for(int j=NAds[P][H1][i].size()-2; j>=0; j--) {
                    float H2 = this->H2[i];
                    float H3 = this->H3[j];

                    bool doCopyI = false;
                    bool doCopyJ = false;
                    if(H2<H1 && this->H2[i+1]>=H1) {
                        doCopyI = true;
                    }
                    if(H3<H2 && this->H3[j+1]>=H2) {
                        doCopyJ = true;
                    }

                    if(doCopyI || doCopyJ) {
                        if(debug) qDebug() << H1 << ", " << this->H2[i] << ", " << this->H3[j];
                        int indexI = i+doCopyI;
                        int indexJ = j+doCopyJ;
                        NAds[P][H1][i][j] = NAds[P][H1][indexI][indexJ];
                        NDes[P][H1][i][j] = NDes[P][H1][indexI][indexJ];
                        CAds[P][H1][i][j] = CAds[P][H1][indexI][indexJ];
                        CDes[P][H1][i][j] = CDes[P][H1][indexI][indexJ];
                        if(debug) {
                            qDebug() << "NAds["<<P<<"]["<<H1<<"]["<<i<<"]["<<j<<"] = " << NAds[P][H1][i][j];
                            qDebug() << "NDes["<<P<<"]["<<H1<<"]["<<i<<"]["<<j<<"] = " << NDes[P][H1][i][j];
                            qDebug() << "CAds["<<P<<"]["<<H1<<"]["<<i<<"]["<<j<<"] = " << CAds[P][H1][i][j];
                            qDebug() << "CDes["<<P<<"]["<<H1<<"]["<<i<<"]["<<j<<"] = " << CDes[P][H1][i][j];
                        }
                    }
                }
            }

            NAds[P][H1].push_back(NAds[P][H1].last());
            NDes[P][H1].push_back(NDes[P][H1].last());
            CAds[P][H1].push_back(CAds[P][H1].last());
            CDes[P][H1].push_back(CDes[P][H1].last());
        }
    }

    if(debug) qDebug() << "Delta value: " << deltaPoreSize;
}

float AdsDesReader::getNum(QString mode, QString P, float H1, float H2, float H3) {
    float poreVolume = H1*H2*H3;

    auto &data = CAds[P];
    if(mode == QString("desorption")) {
        // qDebug() << "Choosing desorption";
        data = CDes[P];
    } else {
        // qDebug() << "Choosing adsorption";
    }

    auto &dataReal = NAds[P];
    if(mode == QString("desorption")) {
        dataReal = NDes[P];
    }
    int idx1 = int(H1);
    if(idx1>17) idx1 = 17;
    int idx2 = idx1+1;
    float fraction = (idx2-H1); // 13.23-13 = 0.23
    auto &data1 = data.at(idx1);
    auto &data2 = data.at(idx2);

    auto &dataReal1 = dataReal.at(idx1);
    auto &dataReal2 = dataReal.at(idx2);

    float H2AdjustedPoreSize = H2-2; // - 2 because pore sizes start at 2
    int m = H2AdjustedPoreSize*oneOverDeltaPoreSize;
    float mf = ( this->H2.at(m+1) - H2)*oneOverDeltaPoreSize;

    float H3AdjustedPoreSize = H3-2; // - 2 because pore sizes start at 2
    int n = H3AdjustedPoreSize*oneOverDeltaPoreSize; // - 2 because pore sizes start at 2
    float nf = ( this->H3.at(n+1) - H3)*oneOverDeltaPoreSize;

    // Calculate interpolated estimated
    float N_ads0 = (mf*nf*data1.at(m).at(n) + mf*(1.0-nf)*data1.at(m).at(n+1)
            + (1.0-mf)*nf*data1.at(m+1).at(n) + (1.0-mf)*(1.0-nf)*data1.at(m+1).at(n+1)) * poreVolume;

    float N_ads1 = (mf*nf*data2.at(m).at(n) + mf*(1.0-nf)*data2.at(m).at(n+1)
            + (1.0-mf)*nf*data2.at(m+1).at(n) + (1.0-mf)*(1.0-nf)*data2.at(m+1).at(n+1)) * poreVolume;

    float N_ads = N_ads0*fraction + (1.0 - fraction)*N_ads1;
    if(debug) {
        float H2Interpolated = mf*this->H2.at(m) + (1.0 - mf)*this->H2.at(m+1);
        float H3Interpolated = nf*this->H3.at(n) + (1.0 - nf)*this->H3.at(n+1);
        qDebug() << "Getting num for " << mode << " with P=" << P << ", H1=" << H1 << ", H2=" << H2 << ", H3=" << H3;
        qDebug() << "To debug, run";
        qDebug() << "reader.printData(" << mode << ", " << P << ", " << H1 << ", " << H2 << ", " << H3 << ");";
        qDebug() << "Pore volume: " << poreVolume;
        qDebug() << "Delta = : " << deltaPoreSize;
        qDebug() << "idx1=" << idx1;
        qDebug() << "idx2=" << idx2;
        qDebug() << "fraction=" << fraction;
        qDebug() << "m (H2Index)=" << m << " which corresponds to H2=" << this->H2[m];
        qDebug() << "mf = ( " << this->H2[m+1] << " - " << H2 << ")/" << deltaPoreSize << " = ( " << this->H2[m+1] - H2 << ")/" << deltaPoreSize << " = " << mf;
        qDebug() << "n (H3Index)=" << n << " which corresponds to H3=" << this->H3[n];
        qDebug() << "nf = ( " << this->H3[n+1] << " - " << H3 << ")/" << deltaPoreSize << " = ( " << this->H3[n+1]-H3 << ")/" << deltaPoreSize << " = " << nf;
        qDebug() << "data1[m][n]=" << data1[m][n];
        qDebug() << "data1[m][n+1]=" << data1[m][n+1];
        qDebug() << "data1[m+1][n]=" << data1[m+1][n];
        qDebug() << "data1[m+1][n+1]=" << data1[m+1][n+1];
        qDebug() << "data2[m][n]=" << data2[m][n];
        qDebug() << "data2[m][n+1]=" << data2[m][n+1];
        qDebug() << "data2[m+1][n]=" << data2[m+1][n];
        qDebug() << "data2[m+1][n+1]=" << data2[m+1][n+1];

        qDebug() << "dataReal1[m][n]=" << dataReal1[m][n];
        qDebug() << "dataReal1[m][n+1]=" << dataReal1[m][n+1];
        qDebug() << "dataReal1[m+1][n]=" << dataReal1[m+1][n];
        qDebug() << "dataReal1[m+1][n+1]=" << dataReal1[m+1][n+1];
        qDebug() << "dataReal2[m][n]=" << dataReal2[m][n];
        qDebug() << "dataReal2[m][n+1]=" << dataReal2[m][n+1];
        qDebug() << "dataReal2[m+1][n]=" << dataReal2[m+1][n];
        qDebug() << "dataReal2[m+1][n+1]=" << dataReal2[m+1][n+1];

        qDebug() << "float N_ads0 = ((" << 1-nf << ")*(" << 1-mf << ")*" << data1[m][n]<< " + " << nf << "*(" << 1-mf << ")*" << data1[m][n+1] << " + (" << 1-nf << ")*" << mf << "*" << data1[m+1][n] << " + " << nf << "*" << mf << "*" << data1[m+1][n+1] << ") * " << poreVolume << " = " << N_ads0;
        qDebug() << "float N_ads1 = ((" << 1-nf << ")*(" << 1-mf << ")*" << data2[m][n]<< " + " << nf << "*(" << 1-mf << ")*" << data2[m][n+1] << " + (" << 1-nf << ")*" << mf << "*" << data2[m+1][n] << " + " << nf << "*" << mf << "*" << data2[m+1][n+1] << ") * " << poreVolume << " = " << N_ads1;
        qDebug() << "N_ads0=" << N_ads0;
        qDebug() << "N_ads1=" << N_ads1;
        qDebug() << "N_ads=" << N_ads;
        if(fabs(mf) > 1.001 || fabs(nf) > 1.001) {
            qDebug() << "We have problems with fractions being larger than 1...";
            exit(0);
        }
    }
    return N_ads;
}

float AdsDesReader::getNumAdsorbed(QString P, float H1, float H2, float H3)
{
    return getNum("adsorption", P, H1, H2, H3);
}

float AdsDesReader::getNumDesorbed(QString P, float H1, float H2, float H3)
{
    return getNum("desorption", P, H1, H2, H3);
}

void AdsDesReader::printData(QString mode, QString P, int H1, float H2, float H3)
{
    for(int i=0; i<NAds[P][H1].size(); i++) {
        float H2Value = this->H2[i];
        if(fabs(H2-H2Value) > 1) continue;
        for(int j=0; j<NAds[P][H1][i].size(); j++) {
            float H3Value = this->H3[j];
            if(fabs(H3-H3Value) > 1) continue;
            if(mode=="adsorption") {
                qDebug() << "H1=" << H1 << ", H2=" << H2Value << ", H3=" << H3Value << ", P=" << P << " has Nads = " << NAds[P][H1][i][j];
            } else {
                qDebug() << "H1=" << H1 << ", H2=" << H2Value << ", H3=" << H3Value << ", P=" << P << " has Nads = " << NDes[P][H1][i][j];
            }
        }
        NAds[P][H1][i].push_back(NAds[P][H1][i].last());
        NDes[P][H1][i].push_back(NAds[P][H1][i].last());
        CAds[P][H1][i].push_back(NAds[P][H1][i].last());
        CDes[P][H1][i].push_back(NAds[P][H1][i].last());
    }
}

void AdsDesReader::readWall(QString mode)
{
    QString fileName = folder+QString("/Nads_%1_wall.txt").arg(mode);
    if(debug) qDebug() << "Reading " << fileName;
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Could not open file " << file.fileName();
        exit(0);
    }

    QStringList lines = QString(file.readAll()).split("\n");
    for(int i=0; i<lines.size(); i++) {
        QString line = lines[i];
        if(line.isEmpty()) continue;
        QString P = pressures[i];
        bool ok;
        float numMolecules = line.toFloat(&ok);
        if(!ok) { qDebug() << "Warning, could not parse " << line; }
        float volume = 5.21128;
        float concentration = numMolecules/volume;
        if(mode=="adsorption") {
            CAdsWall[P] = concentration;
        } else {
            CDesWall[P] = concentration;
        }
    }
}

void AdsDesReader::readFile(int H1, QString P, QString mode)
{
    QString fileName = folder+QString("/Nads_%1_H%2_P%3.txt").arg(mode).arg(H1).arg(P);
    if(debug) qDebug() << "Reading " << fileName;
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Could not open file " << file.fileName();
        exit(0);
    }

    QStringList lines = QString(file.readAll()).split("\n");
    int m = lines.length()-1;
    int n = lines[0].split(" ").length();
    for(int i=0; i<m; i++) {
        QStringList words = lines[i].split(" ");
        for(int j=0; j<n; j++) {
            bool ok;
            float value = words[j].toFloat(&ok);
            if(!ok) { qDebug() << "Warning, could not parse " << words[j]; }
            float v_H1 = H1;
            float v_H2 = H2[i];
            float v_H3 = H2[j];
            float volume = v_H1*v_H2*v_H3;
            if(value != value) {
                value = 0; // NaN
            }

            float concentration = value / volume;
            if(concentration > 1e10) {
                qDebug() << "Damn: value = " << value;
                qDebug() << "Damn: volume = " << volume;
                qDebug() << "H1 = " << v_H1;
                qDebug() << "H2 = " << v_H2;
                qDebug() << "H3 = " << v_H3;
                exit(0);
            }
            if(mode=="adsorption") {
                NAds[P][H1][i].push_back(value);
                CAds[P][H1][i].push_back(concentration);
            } else {
                NDes[P][H1][i].push_back(value);
                CDes[P][H1][i].push_back(concentration);
            }
        }
    }
}
