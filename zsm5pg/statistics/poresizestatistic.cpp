#include "poresizestatistic.h"
#include <cmath>
#include <QDebug>
PoreSizeStatistic::PoreSizeStatistic()
{

}

void PoreSizeStatistic::compute(Zsm5geometry *geometry)
{
    if(!geometry || !geometry->dirty()) return;
    QVector<float> &x = geometry->deltaXVector();
    QVector<float> &y = geometry->deltaYVector();
    QVector<float> &z = geometry->deltaZVector();
    int numberOfPores = x.size()*y.size()*z.size();
    m_poreVolumes.resize(numberOfPores);
    m_histogramValues.resize(numberOfPores);
    int poreIndex = 0;
    for(int i=0; i<x.size(); i++) {
        const float dx = x[i];
        for(int j=0; j<y.size(); j++) {
            const float dy = y[j];
            for(int k=0; k<z.size(); k++) {
                const float dz = z[k];
                const float volume = dx*dy*dz;
                m_poreVolumes[poreIndex++] = volume;
            }
        }
    }

    for(int i=0; i<numberOfPores; i++) {
        m_histogramValues[i] = cbrt(m_poreVolumes[i]);
    }
    computeHistogram();
    geometry->setDirty(false);
}
