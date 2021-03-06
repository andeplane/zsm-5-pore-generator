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
    explicit PoreVolumeStatistic(QObject *parent = nullptr);

public:
    virtual void compute(class Geometry *geometry, int timestep) override;
};

#endif // POREVOLUMESTATISTIC_H
