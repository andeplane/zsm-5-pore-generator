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
    vector<float> &x = geometry->planePositionsX();
    vector<float> &y = geometry->planePositionsY();
    vector<float> &z = geometry->planePositionsZ();
    int numberOfPores = (x.size()-1)*(y.size()-1)*(z.size()-1);
    poreVolumes.resize(numberOfPores);
    poreLengths.resize(numberOfPores);
    int poreIndex = 0;
    for(int i=0; i<x.size()-1; i++) {
        const float x1 = x[i];
        const float x2 = x[i+1];
        const float dx = x2-x1;
        for(int j=0; j<y.size()-1; j++) {
            const float y1 = y[j];
            const float y2 = y[j+1];
            const float dy = y2-y1;
            for(int k=0; k<z.size()-1; k++) {
                const float z1 = z[k];
                const float z2 = z[k+1];
                const float dz = z2-z1;
                const float volume = dx*dy*dz;
                poreVolumes[poreIndex++] = volume;
            }
        }
    }

    for(int i=0; i<numberOfPores; i++) {
        poreLengths[i] = cbrt(poreVolumes[i]);
    }
}
