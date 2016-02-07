#include "distributionanalysis.h"
#include <cmath>

int DistributionAnalysis::size() const
{
    return m_size;
}

void DistributionAnalysis::setSize(int size)
{
    m_size = size;
}

DistributionAnalysis::DistributionAnalysis()
{

}


#include <iostream>
using namespace std;
void DistributionAnalysis::updateDistribution(Zsm5geometry &geometry)
{
    histogram.resize(m_size);
    memset(&histogram.front(), 0, histogram.size()*sizeof(int));
    const double maxLength = 10.0;
    const double dx = maxLength / m_size;
    const double oneOverDx = 1.0/dx;
    if(distribution.size() != m_size) {
        distribution.resize(m_size);
        for(int i=0; i<distribution.size(); i++) {
            QPointF &p = distribution[i];
            p.setX(i*dx);
        }
    }

    vector<float> &x = geometry.planePositionsX();
    vector<float> &y = geometry.planePositionsY();
    vector<float> &z = geometry.planePositionsZ();
    int count = 0;
    for(int i=0; i<geometry.planesPerDimension()-1; i++) {
        double x1 = x[i];
        double x2 = x[i+1];
        double dx = x2-x1;
        for(int j=0; j<geometry.planesPerDimension()-1; j++) {
            double y1 = y[j];
            double y2 = y[j+1];
            double dy = y2-y1;
            for(int k=0; k<geometry.planesPerDimension()-1; k++) {
                double z1 = z[k];
                double z2 = z[k+1];
                double dz = z2-z1;
                double volume = dx*dy*dz;
                double length = cbrt(volume);
                int histogramIndex = int(length * oneOverDx);
                if(histogramIndex < m_size && histogramIndex >= 0) {
                    histogram[histogramIndex]++;
                    count++;
                }
            }
        }
    }

    // Normalize
    double normalizationConstant = 1.0/(count*dx);
    currentMean = 0;
    for(int i=0; i<distribution.size(); i++) {
        QPointF &p = distribution[i];
        p.setY(histogram[i]*normalizationConstant);
        currentMean += p.x()*p.y()*dx;
    }
}

double DistributionAnalysis::meanSquareError(Zsm5geometry &geometry) {
    updateDistribution(geometry);

    double error = 0;
    for(int i=0; i<distribution.size(); i++) {
        double delta = distribution[i].y() - wantedDistribution[i].y();
        error += delta*delta;
    }

    return error;
}

void DistributionAnalysis::findGradient(Zsm5geometry &geometry, Zsm5geometry &gradient)
{
    gradient.planePositionsX().resize(geometry.planesPerDimension());
    gradient.planePositionsY().resize(geometry.planesPerDimension());
    gradient.planePositionsZ().resize(geometry.planesPerDimension());

    vector<float> &x = geometry.planePositionsX();
    vector<float> &y = geometry.planePositionsY();
    vector<float> &z = geometry.planePositionsZ();

    vector<float> &dEdx = gradient.planePositionsX();
    vector<float> &dEdy = gradient.planePositionsY();
    vector<float> &dEdz = gradient.planePositionsZ();

    float eps = 1e-5;
    float error = meanSquareError(geometry);
    for(int i=0; i<geometry.planesPerDimension(); i++) {
        x[i] += eps;
        double errorPlus = meanSquareError(geometry);
        x[i] -= eps;
//        x[i] -= 2.0*eps;
//        double errorMinus = meanSquareError(geometry);
//        x[i] += eps; // Restore value
//        double errorDerivative = (errorPlus - errorMinus)/(2.0*eps);
        double errorDerivative = (errorPlus - error)/eps;
        dEdx[i] = errorDerivative;

        y[i] -= eps;
        errorPlus = meanSquareError(geometry);
//        y[i] -= 2.0*eps;
//        errorMinus = meanSquareError(geometry);
//        y[i] += eps; // Restore value
//        errorDerivative = (errorPlus - errorMinus)/(2.0*eps);
        errorDerivative = (errorPlus - error)/eps;
        dEdy[i] = errorDerivative;

        z[i] += eps;
        errorPlus = meanSquareError(geometry);
        z[i] -= eps;
//        z[i] -= 2.0*eps;
//        errorMinus = meanSquareError(geometry);
//        z[i] += eps; // Restore value
//        errorDerivative = (errorPlus - errorMinus)/(2.0*eps);
        errorDerivative = (errorPlus - error)/eps;
        dEdz[i] = errorDerivative;
    }
}

void DistributionAnalysis::updateWantedDistribution(Zsm5geometry &geometry)
{
    wantedDistribution.resize(m_size);
    float avgLengthPerBox = geometry.planeSize() / geometry.planesPerDimension();
    float maxLength = 5.0*avgLengthPerBox;
    float dx = maxLength / m_size;
    float sigma = 0.05*maxLength;
    float mu = 0.4*maxLength;
    wantedMean = 0;
    for(int i=0; i<m_size; i++) {
        float x = dx*i;
        float p = 1.0/(sigma*sqrt(2*M_PI))*exp(-(x-mu)*(x-mu)/(2.0*sigma*sigma));
        wantedMean += x*p*dx;
        wantedDistribution[i].setX(x);
        wantedDistribution[i].setY(p);
    }
}
