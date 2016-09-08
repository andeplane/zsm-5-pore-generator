#ifndef PORESIZESTATISTIC_H
#define PORESIZESTATISTIC_H

#include "statistic.h"
class PoreSizeStatistic : public Statistic
{
    Q_OBJECT
private:
    QVector<float> m_poreVolumes;
    void computeMode0(PlaneGeometry *geometry);
    void computeMode1(PlaneGeometry *geometry);
public:
    PoreSizeStatistic();
public:
    // Statistic interface
public:
    virtual void compute(class PlaneGeometry *geometry) override;
};

#endif // PORESIZESTATISTIC_H
