#ifndef DISTRIBUTIONANALYSIS_H
#define DISTRIBUTIONANALYSIS_H
#include "zsm5geometry.h"
#include <vector>
#include <QVector>
#include <QPointF>

using std::vector;

class DistributionAnalysis
{
private:
    int m_size = 100;
public:
    QVector<QPointF> distribution;
    QVector<QPointF> wantedDistribution;
    vector<vector<vector<float> > > volumes;

    DistributionAnalysis();
    double computeVolume(Zsm5geometry &geometry);
    void updateDistribution(Zsm5geometry &geometry);
    void updateWantedDistribution(Zsm5geometry &geometry);
    int size() const;
    void setSize(int size);
    double meanSquareError();
};

#endif // DISTRIBUTIONANALYSIS_H
