#ifndef LENGTHRATIO_H
#define LENGTHRATIO_H

#include "statistic.h"

class LengthRatio : public Statistic
{
    Q_OBJECT
public:
    explicit LengthRatio(QObject *parent = nullptr);

    // Statistic interface
public:
    virtual void compute(class Geometry *geometry, int timestep) override;
};

#endif // LENGTHRATIO_H
