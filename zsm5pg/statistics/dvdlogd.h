#ifndef DVDLOGD_H
#define DVDLOGD_H

#include "statistic.h"



class DVDLogd : public Statistic
{
    Q_OBJECT
public:
    explicit DVDLogd(QObject *parent = nullptr);

    // Statistic interface
public:
    virtual void compute(class Geometry *geometry, int timestep) override;
};

#endif // DVDLOGD_H
