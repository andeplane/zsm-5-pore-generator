#ifndef PORESIZESTATISTIC_H
#define PORESIZESTATISTIC_H

#include "statistic.h"
#include "zsm5geometry.h"
class PoreSizeStatistic : public Statistic
{
public:
    PoreSizeStatistic();
    Zsm5geometry *geometry = nullptr;
    vector<float> poreVolumes;
    vector<float> poreSizes;
public:
    virtual void compute();
    Zsm5geometry *getGeometry() const;
    void setGeometry(Zsm5geometry *value);
};

#endif // PORESIZESTATISTIC_H
