#ifndef CONCENTRATION_H
#define CONCENTRATION_H

#include "statistic.h"

class Concentration : public Statistic
{
    Q_OBJECT
public:
    Concentration(QString adsorptionMatrixFilename);
    virtual void compute(class PlaneGeometry *geometry) override;
private:
    QVector<float> m_pressures;
    QVector<QVector<float>> m_values;
    QVector<float> m_volumes;
    void computeMode1(class PlaneGeometry *geometry);
    void computeMode0(class PlaneGeometry *geometry);
};

#endif // CONCENTRATION_H
