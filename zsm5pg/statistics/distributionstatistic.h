#ifndef DISTRIBUTIONSTATISTIC_H
#define DISTRIBUTIONSTATISTIC_H

#include "statistic.h"

class DistributionStatistic : public Statistic
{
    Q_OBJECT
public:
    enum class Type {None, Normal, Exponential};

    explicit DistributionStatistic(QObject *parent = nullptr);
    Type type() const;
    void setType(Type type, float xMin, float xMax, int points = 100);

    float normalDistributionMean() const;
    void setNormalDistributionMean(float normalDistributionMean);

    float normalDistributionStandardDeviation() const;
    void setNormalDistributionStandardDeviation(float normalDistributionStandardDeviation);

    float exponentialDistributionMean() const;
    void setExponentialDistributionMean(float exponentialDistributionMean);

signals:
    void typeChanged();
public slots:
    void update();

private:
    Type  m_type = Type::None;
    float m_normalDistributionMean = 0;
    float m_normalDistributionStandardDeviation = 1.0;
    float m_exponentialDistributionMean = 1.0;
};

#endif // DISTRIBUTIONSTATISTIC_H
