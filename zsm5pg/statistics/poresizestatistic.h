#ifndef PORESIZESTATISTIC_H
#define PORESIZESTATISTIC_H

#include "statistic.h"
#include "zsm5geometry.h"
class PoreSizeStatistic : public Statistic
{
    Q_OBJECT
private:
    QVector<float> m_poreVolumes;
public:
    PoreSizeStatistic();
public:
    // Statistic interface
public:
    virtual void compute(Zsm5geometry *geometry) override;
};

#endif // PORESIZESTATISTIC_H
