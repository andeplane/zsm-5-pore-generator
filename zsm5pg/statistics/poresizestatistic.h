#ifndef PORESIZESTATISTIC_H
#define PORESIZESTATISTIC_H

#include "statistic.h"
class PoreSizeStatistic : public Statistic
{
    Q_OBJECT
private:
    QVector<float> m_poreVolumes;
    void computeMode0(class Geometry *geometry);
    void computeMode1(class Geometry *geometry);
public:
    explicit PoreSizeStatistic(QObject *parent = nullptr);
public:
    // Statistic interface
public:
    virtual void compute(class Geometry *geometry, int timestep) override;
};

#endif // PORESIZESTATISTIC_H
