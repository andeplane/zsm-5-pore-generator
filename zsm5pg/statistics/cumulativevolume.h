#ifndef CUMULATIVEVOLUME_H
#define CUMULATIVEVOLUME_H

#include "statistic.h"

class CumulativeVolume : public Statistic
{
    Q_OBJECT
public:
    CumulativeVolume();

    // Statistic interface
public:
    virtual void compute(class Zsm5geometry *geometry) override;
};

#endif // CUMULATIVEVOLUME_H
