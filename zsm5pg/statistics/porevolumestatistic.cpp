#include "porevolumestatistic.h"

PoreVolumeStatistic::PoreVolumeStatistic()
{
    m_name = "PoreVolume";
    m_xLabel = "Pore volume [nm^3]";
    m_yLabel = "P(V)";
}

void PoreVolumeStatistic::compute(Zsm5geometry *geometry)
{
    if(!geometry) return;
    Statistic::compute(geometry);
    QVector<float> &x = geometry->deltaXVector();
    QVector<float> &y = geometry->deltaYVector();
    QVector<float> &z = geometry->deltaZVector();
    int numberOfPores = x.size()*y.size()*z.size();
    m_poreVolumes.resize(numberOfPores);
    // m_histogramValues.resize(numberOfPores);
    int poreIndex = 0;
    for(int i=0; i<x.size(); i++) {
        const float dx = x[i];
        for(int j=0; j<y.size(); j++) {
            const float dy = y[j];
            for(int k=0; k<z.size(); k++) {
                const float dz = z[k];
                const float volume = dx*dy*dz;
                m_histogramValues[poreIndex++] = volume;
            }
        }
    }

    computeHistogram();
}

