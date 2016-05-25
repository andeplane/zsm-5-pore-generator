#ifndef LENGTHRATIO_H
#define LENGTHRATIO_H

#include "statistic.h"

class LengthRatio : public Statistic
{
public:
    LengthRatio();

    // Statistic interface
public:
    virtual void compute(class Zsm5geometry *geometry) override;
};

#endif // LENGTHRATIO_H
