#include "poresizestatistic.h"
#include "../geometry.h"
#include <cmath>
#include <QDebug>
// #define POREISCBRT
PoreSizeStatistic::PoreSizeStatistic(QObject *parent) : Statistic(parent)
{
    m_name = "poreSizeDistribution";
    m_xLabel = "Pore size [nm]";
    m_yLabel = "P(d)";
    setIsValid(true);
}

void PoreSizeStatistic::computeMode0(Geometry *geometry)
{
    qDebug() << "Computing mode 0";
    QVector<float> &x = geometry->deltaXVector();
    QVector<float> &y = geometry->deltaYVector();
    QVector<float> &z = geometry->deltaZVector();
    int numberOfPores = x.size()*y.size()*z.size();
    m_poreVolumes.resize(numberOfPores);
    int poreIndex = 0;
    for(int i=0; i<x.size(); i++) {
        const float dx = x[i];
        for(int j=0; j<y.size(); j++) {
            const float dy = y[j];
            for(int k=0; k<z.size(); k++) {
                const float dz = z[k];

                float poreSize = std::min(std::min(dx,dy), dz);
                m_poreVolumes[poreIndex++] = poreSize;
            }
        }
    }

    m_histogramValues.clear();
    for(int i=0; i<numberOfPores; i++) {
        m_histogramValues.push_back(m_poreVolumes[i]);
    }
    computeHistogram();
}

void PoreSizeStatistic::computeMode1(Geometry *geometry) {
    QVector<float> &x = geometry->deltaXVector();
    QVector<float> &y = geometry->deltaYVector();
    QVector<float> &z = geometry->deltaZVector();
    int numberOfPores = x.size() + y.size() + z.size();
    m_poreVolumes.resize(numberOfPores);
    int poreIndex = 0;
    for(int i=0; i<x.size(); i++) {
        float poreSizes[3];
        poreSizes[0] = x[i];
        poreSizes[1] = y[i];
        poreSizes[2] = z[i];
        for(int a=0; a<3; a++) {
            const float poreSize = poreSizes[a];
            // const float volume = poreSize*poreSize*poreSize;
            m_poreVolumes[poreIndex++] = poreSize;
        }
    }

    for(int i=0; i<numberOfPores; i++) {
        m_histogramValues.push_back(m_poreVolumes[i]);
    }
    computeHistogram();
    updateQML();
}

void PoreSizeStatistic::compute(Geometry *geometry, int timestep)
{
    if(!geometry || m_lastComputed == timestep) return;

    Statistic::compute(geometry, timestep);
    if(m_mode==0) computeMode0(geometry);
    if(m_mode==1) computeMode1(geometry);
    m_lastComputed = timestep;
}
