#include "dvdlogd.h"
#include "../zsm5geometry.h"
#include <gsl/gsl_vector.h>
#include <gsl/gsl_sort_vector.h>
#include <cmath>
#include <QDebug>

DVDLogd::DVDLogd(QObject *parent) : Statistic(parent)
{
    m_name = "DVDLogD";
    m_xLabel = "Pore size [nm]";
    m_yLabel = "dV/dlogd";
}


void DVDLogd::compute(Geometry *geometry)
{
    if(!geometry) return;

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

    int skipping = 0;
    for(int i=0; i<numberOfPores; i++) {
        float dVN = gsl_vector_get(poreVolumesNormalized, i); // dVN deltaVolumeNormalized
        float poreSize = gsl_vector_get(poreLengths, i);
        int bin = poreSize / dx;
        if(bin>=bins()) {
            skipping++;
            continue; // Some pore sizes might be larger than largest? Don't seg fault
        }
        m_points[bin].setY(m_points[bin].y() + dVN);
    }

    for(int i=1; i<bins(); i++) {
        qreal newValue = m_points[i].y() + m_points[i-1].y();
        m_points[i].setY(newValue);
    }

    QList<QPointF> oldValues = m_points;
    for(int i=1; i<bins(); i++) {
        const qreal &x0 = oldValues[i-1].x();
        const qreal &x1 = oldValues[i].x();
        const qreal &y0 = oldValues[i-1].y();
        const qreal &y1 = oldValues[i].y();

        float dy = y1-y0;
        float dx = x1-x0;
        float meanX = 0.5*(x0+x1);
        m_points[i].setY(meanX*dy/dx);
    }
}
