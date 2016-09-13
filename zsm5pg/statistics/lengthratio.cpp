#include "lengthratio.h"
#include "../zsm5geometry.h"
#include <QDebug>
LengthRatio::LengthRatio(QObject *parent) : Statistic(parent)
{
    m_xLabel = "Pore length ratio";
    m_yLabel = "Probability";
    m_name = "Length ratio";
}


void LengthRatio::compute(Geometry *geometry)
{
    if(!geometry) return;
    QVector<float> &x = geometry->deltaXVector();
    QVector<float> &y = geometry->deltaYVector();
    QVector<float> &z = geometry->deltaZVector();

    int numberOfPores = x.size()*y.size()*z.size();
    m_histogramValues.clear();
    float min = 1e9;
    float max = 0;
    int poreIndex = 0;
    for(int i=0; i<x.size(); i++) {
        const float dx = x[i];
        for(int j=0; j<y.size(); j++) {
            const float dy = y[j];
            for(int k=0; k<z.size(); k++) {
                const float dz = z[k];
                float dxdy = dx/dy;
                dxdy = std::max(dxdy, 1.0f/dxdy);

                float dxdz = dx/dz;
                dxdz = std::max(dxdz, 1.0f/dxdz);

                float dydz = dy/dz;
                dydz = std::max(dydz, 1.0f/dydz);

                const float maxRatio = std::max(std::max(dxdy, dxdz), dydz);
                m_histogramValues.push_back(maxRatio);
                min = std::min(min, maxRatio);
                max = std::max(max, maxRatio);
            }
        }
    }

    setMin(min);
    setMax(max);

    computeHistogram();
}
