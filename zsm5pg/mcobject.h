#ifndef MCOBJECT_H
#define MCOBJECT_H

#include <QFile>
#include <QObject>

class MCObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(double value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(double standardDeviation READ standardDeviation WRITE setStandardDeviation NOTIFY standardDeviationChanged)
public:
    explicit MCObject(QObject *parent = 0);
    double value() const;
    double standardDeviation() const;
    void randomWalk();
    void storeCurrent();
    void rejectRW();
    virtual void saveState(QFile &file);
    virtual void loadState(class IniFile *iniFile);
    QString name() const;

signals:
    void valueChanged(double value);
    void standardDeviationChanged(double standardDeviation);
    void nameChanged(QString name);

public slots:
    void setValue(double value);
    void setStandardDeviation(double standardDeviation);
    void setName(QString name);

private:
    double m_value = 0.0;
    double m_valueStored = 0.0;
    double m_standardDeviation = 1.0;
    QString m_name;
};

#endif // MCOBJECT_H
