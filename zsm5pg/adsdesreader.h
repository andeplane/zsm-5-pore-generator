#ifndef ADSDESREADER_H
#define ADSDESREADER_H

#include <QString>
#include <QStringList>
#include <QVector>
#include <QMap>

class AdsDesReader
{
public:
    int largestPoreSize;
    int resampleSize;
    float oneOverDeltaPoreSize;
    float deltaPoreSize;
    bool debug;
    QStringList modes;
    QString folder;
    QVector<float> H2;
    QVector<float> H3;
    QMap<QString, QVector<QVector<QVector<float>>>> NAds;
    QMap<QString, QVector<QVector<QVector<float>>>> NDes;
    QMap<QString, QVector<QVector<QVector<float>>>> CAds;
    QMap<QString, QVector<QVector<QVector<float>>>> CDes;

    QMap<QString, float> CAdsWall;
    QMap<QString, float> CDesWall;
    QStringList pressures = {"0.000100", "0.000500", "0.001000", "0.059765", "0.118529", "0.177294", "0.236059", "0.294824", "0.353588", "0.412353", "0.471118", "0.529882", "0.588647", "0.647412", "0.706176", "0.764941", "0.823706", "0.882471", "0.941235"};
    // Functions
    AdsDesReader();
    void readFile(int H1, QString P, QString mode);
    void readH2H3Matrices();
    void readData();
    void appendDummyData();;
    float getNum(const QVector<QVector<QVector<float>>> &data, const QVector<QVector<QVector<float>>> &dataReal, float H1, float H2, float H3) const;
    float getNumAdsorbed(QString P, float H1, float H2, float H3);
    float getNumDesorbed(QString P, float H1, float H2, float H3);
    void printData(QString mode, QString P, int H1, float H2, float H3);
    void readWall(QString mode);
};

#endif // ADSDESREADER_H
