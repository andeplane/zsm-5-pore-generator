#ifndef CUMULATIVEVOLUME_H
#define CUMULATIVEVOLUME_H

#include "statistic.h"

class CumulativeVolume : public Statistic
{
    Q_OBJECT
public:
    explicit CumulativeVolume(QObject *parent = nullptr);

    // Statistic interface
public:
    virtual void compute(class Geometry *geometry) override;
};

#endif // CUMULATIVEVOLUME_H
