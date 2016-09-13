#include "poresizestatistic.h"
#include <cmath>
#include <QDebug>
// #define POREISCBRT
PoreSizeStatistic::PoreSizeStatistic(QObject *parent) : Statistic(parent)
{
    m_name = "PoreSize";
    m_xLabel = "Pore size [nm]";
    m_yLabel = "P(d)";
}

void PoreSizeStatistic::computeMode0(Geometry *geometry)
{
    if(!geometry) return;
    Statistic::compute(geometry);
    qDebug() << "Histogram size: " << m_histogramValues.size();
    QVector<float> &x = geometry->deltaXVector();
    QVector<float> &y = geometry->deltaYVector();
    QVector<float> &z = geometry->deltaZVector();
    int numberOfPores = x.size()*y.size()*z.size();
    m_poreVolumes.resize(numberOfPores);
    // m_histogramValues.resize(numberOfPores);
    // m_histogramValues.resize(numberOfPores);
    qDebug() << "Number of pores: " << numberOfPores;
    int poreIndex = 0;
    for(int i=0; i<x.size(); i++) {
        const float dx = x[i];
        for(int j=0; j<y.size(); j++) {
            const float dy = y[j];
            for(int k=0; k<z.size(); k++) {
                const float dz = z[k];
                const float volume = dx*dy*dz;

                float small = dx;
                float medium = dy;
                float large = dz;

                if(small > medium) std::swap(small, medium);
                if(small > large) std::swap(small, large);
                if(medium > large) std::swap(medium, large);

                float poreSize = std::min(std::min(dx,dy), dz);
                // poreSize = 0.5*(small + medium);
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
        m_histogramValues.push_back(m_poreVolumes[i]);
    }
    computeHistogram();
    updateQML();
//    qDebug() << m_xValuesRaw;
//    qDebug() << m_yValuesRaw;
}

void PoreSizeStatistic::computeMode1(Geometry *geometry) {
    Statistic::compute(geometry);
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

void PoreSizeStatistic::compute(Geometry *geometry)
{
    if(m_mode==0) computeMode0(geometry);
    if(m_mode==1) computeMode1(geometry);
}
