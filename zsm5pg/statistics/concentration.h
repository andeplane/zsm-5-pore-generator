#ifndef CONCENTRATION_H
#define CONCENTRATION_H

#include "statistic.h"

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

public slots:

signals:
    void zeoliteThicknessChanged(double zeoliteThickness);
    void scalingFactorChanged(double scalingFactor);
    void adsorptionChanged(bool adsorption);

private:
    bool isValid();
    QVector<float> m_pressures;
    QVector<QVector<float>> m_values;
    QVector<float> m_volumes;
    void computeMode0(class Geometry *geometry);
    void computeMode1(class Geometry *geometry);
    void readFile(QString fileName);
    double m_scalingFactor = 1.5;
    double m_zeoliteThickness = 1.24;
    // Statistic interface
    double findNumAdsorbed(double P, double Lx, double Ly, double Lz);
    bool m_adsorption = false;

public slots:
    virtual void loadIniFile(class IniFile *iniFile) override;
    virtual void saveState(QFile &file) override;
    void setZeoliteThickness(double zeoliteThickness);
    void setScalingFactor(double scalingFactor);
    void setAdsorption(bool adsorption);
};

#endif // CONCENTRATION_H
