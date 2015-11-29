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

double DistributionAnalysis::computeVolume(Zsm5geometry &geometry) {
    double volume = 0;
    volumes.resize(geometry.planesPerDimension()-1, vector<vector<float> >(geometry.planesPerDimension()-1, vector<float>(geometry.planesPerDimension()-1, 0)));
    vector<float> &x = geometry.planePositionsX();
    vector<float> &y = geometry.planePositionsY();
    vector<float> &z = geometry.planePositionsZ();
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
                double volumeBox = dx*dy*dz;
                volumes[i][j][k] = volumeBox;
                volume += volumeBox;
            }
        }
    }

    return volume;
}
#include <iostream>
using namespace std;
void DistributionAnalysis::updateDistribution(Zsm5geometry &geometry)
{
    computeVolume(geometry);
    distribution.resize(m_size);
    double avgLengthPerBox = geometry.planeSize() / geometry.planesPerDimension();
    double maxLength = 5.0*avgLengthPerBox;
    double dx = maxLength / distribution.size();
    double oneOverDx = 1.0/dx;
    double numberOfVolumes = powf(geometry.planesPerDimension()-1,3);
    for(int i=0; i<distribution.size(); i++) {
        QPointF &p = distribution[i];
        p.setX(i*dx);
        p.setY(0);
    }
    // Create histogram by counting
    for(int i=0; i<geometry.planesPerDimension()-1; i++) {
        for(int j=0; j<geometry.planesPerDimension()-1; j++) {
            for(int k=0; k<geometry.planesPerDimension()-1; k++) {
                double volume = volumes[i][j][k];
                double length = powf(volume, 1.0/3.0);
                int histogramIndex = length * oneOverDx;
                if(histogramIndex < distribution.size()) {
                    QPointF &p = distribution[histogramIndex];
                    p.setY(p.y()+1);
                }
            }
        }
    }

    // Normalize
    for(QPointF &p : distribution) {
        p.setY(p.y()/(numberOfVolumes*dx));
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

    float eps = 1e-3;

    for(int i=0; i<geometry.planesPerDimension(); i++) {
        x[i] += eps;
        double errorPlus = meanSquareError(geometry);
        x[i] -= 2.0*eps;
        double errorMinus = meanSquareError(geometry);
        x[i] += eps; // Restore value
        double errorDerivative = (errorPlus - errorMinus)/(2.0*eps);
        dEdx[i] = errorDerivative;

        y[i] += eps;
        errorPlus = meanSquareError(geometry);
        y[i] -= 2.0*eps;
        errorMinus = meanSquareError(geometry);
        y[i] += eps; // Restore value
        errorDerivative = (errorPlus - errorMinus)/(2.0*eps);
        dEdy[i] = errorDerivative;

        z[i] += eps;
        errorPlus = meanSquareError(geometry);
        z[i] -= 2.0*eps;
        errorMinus = meanSquareError(geometry);
        z[i] += eps; // Restore value
        errorDerivative = (errorPlus - errorMinus)/(2.0*eps);
        dEdz[i] = errorDerivative;
    }
}

void DistributionAnalysis::updateWantedDistribution(Zsm5geometry &geometry)
{
    wantedDistribution.resize(m_size);
    float avgLengthPerBox = geometry.planeSize() / geometry.planesPerDimension();
    float maxLength = 5.0*avgLengthPerBox;
    float dx = maxLength / m_size;
    float sigma = 0.1*maxLength;
    float mu = 0.3*maxLength;
    for(int i=0; i<m_size; i++) {
        float x = dx*i;
        float p = 1.0/(sigma*sqrt(2*M_PI))*exp(-(x-mu)*(x-mu)/(2.0*sigma*sigma));
        wantedDistribution[i].setX(x);
        wantedDistribution[i].setY(p);
    }
}
