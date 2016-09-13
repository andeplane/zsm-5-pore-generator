#ifndef PORESIZESTATISTIC_H
#define PORESIZESTATISTIC_H

#include "statistic.h"
#include "zsm5geometry.h"
class PoreSizeStatistic : public Statistic
{
    Q_OBJECT
private:
    QVector<float> m_poreVolumes;
    void computeMode0(Geometry *geometry);
    void computeMode1(Geometry *geometry);
public:
    explicit PoreSizeStatistic(QObject *parent = nullptr);
public:
    // Statistic interface
public:
    virtual void compute(Geometry *geometry) override;
};

#endif // PORESIZESTATISTIC_H
