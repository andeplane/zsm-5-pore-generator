#include "poresizestatistic.h"
#include <cmath>

PoreSizeStatistic::PoreSizeStatistic()
{

}

Zsm5geometry *PoreSizeStatistic::getGeometry() const
{
    return geometry;
}

void PoreSizeStatistic::setGeometry(Zsm5geometry *value)
{
    geometry = value;
}

void PoreSizeStatistic::compute()
{
    vector<float> &x = geometry->deltaXVector();
    vector<float> &y = geometry->deltaYVector();
    vector<float> &z = geometry->deltaZVector();
    int numberOfPores = x.size()*y.size()*z.size();
    m_poreVolumes.resize(numberOfPores);
    m_histogramValues.resize(numberOfPores);
    int poreIndex = 0;
    for(unsigned long i=0; i<x.size(); i++) {
        const float dx = x[i];
        for(unsigned long j=0; j<y.size(); j++) {
            const float dy = y[j];
            for(unsigned long k=0; k<z.size(); k++) {
                const float dz = z[k];
                const float volume = dx*dy*dz;
                m_poreVolumes[poreIndex++] = volume;
            }
        }
    }

    for(int i=0; i<numberOfPores; i++) {
        m_histogramValues[i] = cbrt(m_poreVolumes[i]);
    }
}
