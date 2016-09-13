#ifndef DVDLOGD_H
#define DVDLOGD_H

#include "statistic.h"



class DVDLogd : public Statistic
{
    Q_OBJECT
public:
    DVDLogd();

    // Statistic interface
public:
    virtual void compute(class Geometry *geometry) override;
};

#endif // DVDLOGD_H
