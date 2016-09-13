#ifndef STATISTIC_H
#define STATISTIC_H

#include <QObject>
#include <QVector>
#include <QVariantList>
#include <QLineSeries>
#include <QUrl>
using namespace QtCharts;

class Statistic : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString xLabel READ xLabel WRITE setXLabel NOTIFY xLabelChanged)
    Q_PROPERTY(QString yLabel READ yLabel WRITE setYLabel NOTIFY yLabelChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QList<QPointF> points READ points WRITE setPoints NOTIFY pointsChanged)
    Q_PROPERTY(int histogramAverageCount READ histogramAverageCount WRITE setHistogramAverageCount NOTIFY histogramAverageCountChanged)
    Q_PROPERTY(float min READ min WRITE setMin NOTIFY minChanged)
    Q_PROPERTY(float max READ max WRITE setMax NOTIFY maxChanged)
    Q_PROPERTY(int bins READ bins WRITE setBins NOTIFY binsChanged)
    Q_PROPERTY(int mode READ mode WRITE setMode NOTIFY modeChanged)
    Q_PROPERTY(bool isValid READ isValid WRITE setIsValid NOTIFY isValidChanged)
private:
    void normalizeHistogram();
    float m_min = 0;
    float m_max = 10;
    int m_histogramAverageCount;
    int m_timesteps = 0;
    bool m_isValid = false;

protected:
    int m_mode = 0;
    int m_bins = 100;
    QList<float> m_histogramValues;
    QList<QPointF> m_points;
    QString m_name;
    QString m_xLabel;
    QString m_yLabel;

public:
    explicit Statistic(QObject *parent = nullptr);
    virtual void compute(class Geometry *geometry);
    void computeHistogram();
    int bins() const;
    float min() const;
    float max() const;
    void save(QString filename);
    void load(QString filename);
    double chiSquared(Statistic *statistic);
    virtual double eval(double x);
    void emitReady();
    void updateQML();
    QString xLabel() const;
    QString yLabel() const;
    QString name() const;
    QList<QPointF> &points();
    Q_INVOKABLE void saveHistogramValues(QUrl filename);
    Q_INVOKABLE void updateSeries(QAbstractSeries *series);
    int histogramAverageCount() const;
    int mode() const;
    bool isValid() const;

public slots:
    void setBins(int bins);
    void setMin(float min);
    void setMax(float max);
    void setXLabel(QString xLabel);
    void setYLabel(QString yLabel);
    void setName(QString name);
    void setPoints(QList<QPointF> points);
    void setHistogramAverageCount(int histogramAverageCount);
    void setMode(int mode);
    virtual void loadIniFile(class IniFile *iniFile);
    void setIsValid(bool isValid);

signals:
    void binsChanged(int bins);
    void histogramReady();
    void minChanged(float min);
    void maxChanged(float max);
    void xLabelChanged(QString xLabel);
    void yLabelChanged(QString yLabel);
    void nameChanged(QString name);
    void pointsChanged(QList<QPointF> points);
    void histogramAverageCountChanged(int histogramAverageCount);
    void modeChanged(int mode);
    void isValidChanged(bool isValid);
};

#endif // STATISTIC_H
