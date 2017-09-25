#ifndef CONCENTRATION_H
#define CONCENTRATION_H

#include "statistic.h"
#include "../adsdesreader.h"

class Concentration : public Statistic
{
    Q_OBJECT
    Q_PROPERTY(double zeoliteThickness READ zeoliteThickness WRITE setZeoliteThickness NOTIFY zeoliteThicknessChanged)
    Q_PROPERTY(double scalingFactor READ scalingFactor WRITE setScalingFactor NOTIFY scalingFactorChanged)
    Q_PROPERTY(bool adsorption READ adsorption WRITE setAdsorption NOTIFY adsorptionChanged)
public:
    explicit Concentration(QObject *parent = nullptr);
    virtual void compute(class Geometry *geometry, int timestep) override;
    double zeoliteThickness() const;
    double scalingFactor() const;
    bool adsorption() const;
    void readJan17Files();

public slots:

signals:
    void zeoliteThicknessChanged(double zeoliteThickness);
    void scalingFactorChanged(double scalingFactor);
    void adsorptionChanged(bool adsorption);

private:
    QStringList m_pressureStrings = {"0.000100", "0.000500", "0.001000", "0.059765", "0.118529", "0.177294", "0.236059", "0.294824", "0.353588", "0.412353", "0.471118", "0.529882", "0.588647", "0.647412", "0.706176", "0.764941", "0.823706", "0.882471", "0.941235"};
    bool isValid();
    QVector<float> m_pressures;
    QVector<QVector<float>> m_values;
    QVector<float> m_volumes;
    void computeMode0(class Geometry *geometry);
    void computeMode1(class Geometry *geometry);
    void readFileOct16(QString fileName);
    void readFileJan17(QString fileName);
    double m_scalingFactor = 1.5;
    double m_zeoliteThickness = 1.24;
    // Statistic interface
    double findNumAdsorbedOct16(double P, double Lx, double Ly, double Lz);
    double findNumAdsorbedJan17(QString mode, QString P, double Lx, double Ly, double Lz);
    bool m_adsorption = false;
    AdsDesReader reader;
public slots:
    virtual void loadIniFile(class IniFile *iniFile) override;
    virtual void saveState(QFile &file) override;
    void setZeoliteThickness(double zeoliteThickness);
    void setScalingFactor(double scalingFactor);
    void setAdsorption(bool adsorption);
};

#endif // CONCENTRATION_H
