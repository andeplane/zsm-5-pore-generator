#include "poresizestatistic.h"
#include <cmath>
#include <QDebug>
PoreSizeStatistic::PoreSizeStatistic()
{
    m_name = "PoreSize";
}

void PoreSizeStatistic::compute(Zsm5geometry *geometry)
{
    if(!geometry) return;
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
                const float poreSize = std::min(std::min(dx,dy), dz);
                m_poreVolumes[poreIndex++] = cbrt(volume);
                //m_poreVolumes[poreIndex++] = poreSize;
            }
        }
    }

    for(int i=0; i<numberOfPores; i++) {
        // m_histogramValues[i] = cbrt(m_poreVolumes[i]);
        m_histogramValues[i] = m_poreVolumes[i];
    }
    computeHistogram();
    updateQML();
}
