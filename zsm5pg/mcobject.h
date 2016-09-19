#ifndef MCOBJECT_H
#define MCOBJECT_H

#include <QObject>

class MCObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(double standardDeviation READ standardDeviation WRITE setStandardDeviation NOTIFY standardDeviationChanged)
public:
    explicit MCObject(QObject *parent = 0);
    double value() const;
    double standardDeviation() const;
    void randomWalk();
    void storeCurrent();
    void rejectRW();
signals:
    void valueChanged(double value);
    void standardDeviationChanged(double standardDeviation);

public slots:
    void setValue(double value);
    void setStandardDeviation(double standardDeviation);

private:
    double m_value = 0.0;
    double m_valueStored = 0.0;
    double m_standardDeviation = 1.0;
};

#endif // MCOBJECT_H
