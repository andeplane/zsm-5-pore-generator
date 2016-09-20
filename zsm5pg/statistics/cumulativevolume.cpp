#include "cumulativevolume.h"
#include "../geometry.h"
#include <gsl/gsl_vector.h>
#include <gsl/gsl_sort_vector.h>
#include <cmath>
CumulativeVolume::CumulativeVolume(QObject *parent) : Statistic(parent)
{
    setXLabel("Pore size [nm]");
    setYLabel("Cumulative volume");
}

void CumulativeVolume::computeMode0(Geometry *geometry, int timestep) {
    QVector<float> &x = geometry->deltaXVector();
    QVector<float> &y = geometry->deltaYVector();
    QVector<float> &z = geometry->deltaZVector();

    double totalVolume = geometry->totalVolume();
    double oneOverTotalVolume = 1.0 / totalVolume;
    int numberOfPores = x.size()*y.size()*z.size();

    gsl_vector * poreVolumes = gsl_vector_alloc (numberOfPores);
    gsl_vector * poreVolumesNormalized = gsl_vector_alloc (numberOfPores);
    gsl_vector * poreLengths = gsl_vector_alloc(numberOfPores);
    int poreIndex = 0;
    for(int i=0; i<x.size(); i++) {
        const float dx = x[i];
        for(int j=0; j<y.size(); j++) {
            const float dy = y[j];
            for(int k=0; k<z.size(); k++) {
                const float dz = z[k];
                const float dV = dx*dy*dz;
                float poreLength = std::min(std::min(dx, dy), dz);
#ifdef POREISCBRT
                poreLength = cbrt(dV);
#endif
                gsl_vector_set(poreVolumes, poreIndex, dV);
                gsl_vector_set(poreLengths, poreIndex, poreLength);
                gsl_vector_set(poreVolumesNormalized, poreIndex, dV * oneOverTotalVolume);
                poreIndex++;
            }
        }
    }

    gsl_sort_vector2(poreVolumes, poreLengths); // Sort both vectors based on the first
    gsl_sort_vector(poreVolumesNormalized);

    // Set the x values and be ready to make plot data
    m_points.clear();
    m_points.reserve(bins());
    float dx = (max() - min()) / (bins() - 1);
    for(int i=0; i<bins(); i++) {
        float x = min() + i*dx;
        m_points.push_back(QPointF(x,0));
    }

    for(int i=0; i<numberOfPores; i++) {
        float dVN = gsl_vector_get(poreVolumesNormalized, i); // dVN deltaVolumeNormalized
        float poreSize = gsl_vector_get(poreLengths, i);
        int bin = poreSize / dx;
        if(bin>=bins()) continue; // Some pore sizes might be larger than largest? Don't seg fault
        m_points[bin].setY(m_points[bin].y() + dVN);
    }

    for(int i=1; i<bins(); i++) {
        qreal newValue = m_points[i].y() + m_points[i-1].y();
        m_points[i].setY(newValue);
    }


}

void CumulativeVolume::computeMode1(Geometry *geometry, int timestep) {
    QVector<float> &x = geometry->deltaXVector();
    QVector<float> &y = geometry->deltaYVector();
    QVector<float> &z = geometry->deltaZVector();

    double totalVolume = 0;
    for(int i=0; i<x.size(); i++) {
        totalVolume += x[i]*x[i]*x[i];
        totalVolume += y[i]*y[i]*y[i];
        totalVolume += z[i]*z[i]*z[i];
    }

    double oneOverTotalVolume = 1.0 / totalVolume;
    int numberOfPores = 3*x.size();

    gsl_vector * poreVolumes = gsl_vector_alloc (numberOfPores);
    gsl_vector * poreVolumesNormalized = gsl_vector_alloc (numberOfPores);
    gsl_vector * poreLengths = gsl_vector_alloc(numberOfPores);
    int poreIndex = 0;
    for(int i=0; i<x.size(); i++) {
        float pores[3];
        pores[0] = x[i];
        pores[1] = y[i];
        pores[2] = z[i];
        for(int a=0; a<3; a++) {
            float poreLength = pores[a];
            const float dV = poreLength*poreLength*poreLength;
            gsl_vector_set(poreVolumes, poreIndex, dV);
            gsl_vector_set(poreLengths, poreIndex, poreLength);
            gsl_vector_set(poreVolumesNormalized, poreIndex, dV * oneOverTotalVolume);
            poreIndex++;
        }
    }

    gsl_sort_vector2(poreVolumes, poreLengths); // Sort both vectors based on the first
    gsl_sort_vector(poreVolumesNormalized);

    // Set the x values and be ready to make plot data
    m_points.clear();
    m_points.reserve(bins());
    float dx = (max() - min()) / (bins() - 1);
    for(int i=0; i<bins(); i++) {
        float x = min() + i*dx;
        m_points.push_back(QPointF(x,0));
    }

    for(int i=0; i<numberOfPores; i++) {
        float dVN = gsl_vector_get(poreVolumesNormalized, i); // dVN deltaVolumeNormalized
        float poreSize = gsl_vector_get(poreLengths, i);
        int bin = poreSize / dx;
        if(bin>=bins()) continue; // Some pore sizes might be larger than largest? Don't seg fault
        m_points[bin].setY(m_points[bin].y() + dVN);
    }

    for(int i=1; i<bins(); i++) {
        qreal newValue = m_points[i].y() + m_points[i-1].y();
        m_points[i].setY(newValue);
    }
}

void CumulativeVolume::compute(Geometry *geometry, int timestep)
{
    if(!geometry) return;
    if(m_mode == 0) computeMode0(geometry, timestep);
    else computeMode1(geometry, timestep);
}
