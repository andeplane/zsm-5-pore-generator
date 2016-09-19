#ifndef CONCENTRATION_H
#define CONCENTRATION_H

#include "statistic.h"

class Concentration : public Statistic
{
    Q_OBJECT
    Q_PROPERTY(double zeoliteThickness READ zeoliteThickness WRITE setZeoliteThickness NOTIFY zeoliteThicknessChanged)
    Q_PROPERTY(double scalingFactor READ scalingFactor WRITE setScalingFactor NOTIFY scalingFactorChanged)
public:
    explicit Concentration(QObject *parent = nullptr);
    virtual void compute(class Geometry *geometry, int timestep) override;
    double zeoliteThickness() const;
    double scalingFactor() const;

public slots:

signals:
    void zeoliteThicknessChanged(double zeoliteThickness);
    void scalingFactorChanged(double scalingFactor);

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
public slots:
    virtual void loadIniFile(class IniFile *iniFile) override;
    void setZeoliteThickness(double zeoliteThickness);
    void setScalingFactor(double scalingFactor);
};

#endif // CONCENTRATION_H
