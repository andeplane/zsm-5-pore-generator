#ifndef POREVOLUMESTATISTIC_H
#define POREVOLUMESTATISTIC_H

#include <QVector>
#include "statistic.h"
#include "zsm5geometry.h"
class PoreVolumeStatistic : public Statistic
{
    Q_OBJECT
private:
    QVector<float> m_poreVolumes;

public:
    PoreVolumeStatistic();

public:
    virtual void compute(Zsm5geometry *geometry) override;
};

#endif // POREVOLUMESTATISTIC_H
