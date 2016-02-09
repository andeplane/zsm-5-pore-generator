#ifndef PORESIZESTATISTIC_H
#define PORESIZESTATISTIC_H

#include "statistic.h"
#include "zsm5geometry.h"
class PoreSizeStatistic : public Statistic
{
    Q_OBJECT
private:
    Zsm5geometry *geometry = nullptr;
    vector<float> m_poreVolumes;
public:
    PoreSizeStatistic();
public:
    Zsm5geometry *getGeometry() const;
    void setGeometry(Zsm5geometry *value);

    // Statistic interface
public:
    virtual void compute() override;
};

#endif // PORESIZESTATISTIC_H
