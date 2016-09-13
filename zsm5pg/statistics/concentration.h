#ifndef CONCENTRATION_H
#define CONCENTRATION_H

#include "statistic.h"

class Concentration : public Statistic
{
    Q_OBJECT

public:
    explicit Concentration(QObject *parent = nullptr);
    virtual void compute(Geometry *geometry) override;

public slots:

signals:

private:
    bool isValid();
    QVector<float> m_pressures;
    QVector<QVector<float>> m_values;
    QVector<float> m_volumes;
    void computeMode0(Geometry *geometry);
    void computeMode1(Geometry *geometry);
    void readFile(QString fileName);
    // Statistic interface
public slots:
    virtual void loadIniFile(IniFile *iniFile) override;
};

#endif // CONCENTRATION_H
