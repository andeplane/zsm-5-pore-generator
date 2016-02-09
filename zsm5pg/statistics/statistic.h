#ifndef STATISTIC_H
#define STATISTIC_H

#include <QObject>
#include <QVector>
#include <QVariantList>

class Statistic : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList xValues READ xValues WRITE setXValues NOTIFY xValuesChanged)
    Q_PROPERTY(QVariantList yValues READ yValues WRITE setYValues NOTIFY yValuesChanged)
    Q_PROPERTY(int bins READ bins WRITE setBins NOTIFY binsChanged)
private:
    void normalizeHistogram();

protected:
    int m_bins = 100;
    QVariantList m_xValues;
    QVariantList m_yValues;
    QVector<float> m_xValuesRaw;
    QVector<float> m_yValuesRaw;
    QVector<float> m_histogramValues;

public:
    Statistic();
    void prepareHistogram();
    void computeHistogram();
    QVariantList xValues() const;
    QVariantList yValues() const;
    int bins() const;
    virtual void compute() = 0;

public slots:
    void setXValues(QVariantList xValues);
    void setYValues(QVariantList yValues);
    void setBins(int bins);

signals:
    void xValuesChanged(QVariantList xValues);
    void yValuesChanged(QVariantList yValues);
    void binsChanged(int bins);
    void histogramReady();
};

#endif // STATISTIC_H
