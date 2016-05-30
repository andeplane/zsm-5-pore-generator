#include "poresizestatistic.h"
#include <cmath>
#include <QDebug>
PoreSizeStatistic::PoreSizeStatistic()
{
    m_name = "PoreSize";
    m_xLabel = "Pore size [nm]";
    m_yLabel = "P(d)";
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
                float poreSize = std::min(std::min(dx,dy), dz);
#ifdef POREISCBRT
                poreSize = cbrt(volume);
#endif
                m_poreVolumes[poreIndex++] = poreSize;
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
//    qDebug() << m_xValuesRaw;
//    qDebug() << m_yValuesRaw;
}
