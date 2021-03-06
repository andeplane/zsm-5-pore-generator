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
    virtual void compute(class Geometry *geometry, int timestep) override;
private:
    void computeMode0(class Geometry *geometry, int timestep);
    void computeMode1(class Geometry *geometry, int timestep);
};

#endif // CUMULATIVEVOLUME_H
