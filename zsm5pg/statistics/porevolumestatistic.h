#ifndef POREVOLUMESTATISTIC_H
#define POREVOLUMESTATISTIC_H

#include <QVector>
#include "statistic.h"
class PoreVolumeStatistic : public Statistic
{
    Q_OBJECT
private:
    QVector<float> m_poreVolumes;

public:
    PoreVolumeStatistic();

public:
    virtual void compute(class PlaneGeometry *geometry) override;
};

#endif // POREVOLUMESTATISTIC_H
