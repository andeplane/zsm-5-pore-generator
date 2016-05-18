#ifndef CONCENTRATION_H
#define CONCENTRATION_H

#include "statistic.h"

class Concentration : public Statistic
{
public:
    Concentration(QString adsorptionMatrixFilename);
    virtual void compute(Zsm5geometry *geometry) override;
private:
    QVector<float> m_pressures;
    QVector<QVector<float>> m_values;
    QVector<float> m_volumes;
};

#endif // CONCENTRATION_H
