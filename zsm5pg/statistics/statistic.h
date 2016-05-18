#ifndef STATISTIC_H
#define STATISTIC_H

#include <QObject>
#include <QVector>
#include <QVariantList>
#include <QLineSeries>
using namespace QtCharts;

class Statistic : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList xValues READ xValues WRITE setXValues NOTIFY xValuesChanged)
    Q_PROPERTY(QVariantList yValues READ yValues WRITE setYValues NOTIFY yValuesChanged)
    Q_PROPERTY(float min READ min WRITE setMin NOTIFY minChanged)
    Q_PROPERTY(float max READ max WRITE setMax NOTIFY maxChanged)
    Q_PROPERTY(int bins READ bins WRITE setBins NOTIFY binsChanged)
    Q_PROPERTY(QLineSeries* lineSeries READ lineSeries WRITE setLineSeries NOTIFY lineSeriesChanged)
private:
    void normalizeHistogram();
    float m_min = 0;
    float m_max = 10;
    QLineSeries* m_lineSeries = nullptr;

protected:
    int m_bins = 100;
    QVariantList m_xValues;
    QVariantList m_yValues;
    QVector<float> m_xValuesRaw;
    QVector<float> m_yValuesRaw;
    QVector<float> m_histogramValues;
public:
    QString m_name;
    Statistic();
    virtual void compute(class Zsm5geometry *geometry);
    void prepareHistogram();
    void computeHistogram();
    QVariantList xValues() const;
    QVariantList yValues() const;
    const QVector<float> &xValuesRaw() const;
    const QVector<float> &yValuesRaw() const;
    int bins() const;
    float min() const;
    float max() const;
    void save(QString filename);
    void load(QString filename);
    virtual double eval(double x);
    double chiSquared(Statistic *statistic);
    void emitReady();
    void setXValuesRaw(const QVector<float> &xValuesRaw);
    void setYValuesRaw(const QVector<float> &yValuesRaw);

    QLineSeries* lineSeries() const;
    void createLineSeries();

    void updateQML();
public slots:
    void setXValues(QVariantList xValues);
    void setYValues(QVariantList yValues);
    void setBins(int bins);
    void setMin(float min);
    void setMax(float max);

    void setLineSeries(QLineSeries* lineSeries);

signals:
    void xValuesChanged(QVariantList xValues);
    void yValuesChanged(QVariantList yValues);
    void binsChanged(int bins);
    void histogramReady();
    void minChanged(float min);
    void maxChanged(float max);
    void lineSeriesChanged(QLineSeries* lineSeries);
};

#endif // STATISTIC_H
