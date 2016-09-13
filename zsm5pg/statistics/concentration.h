#ifndef CONCENTRATION_H
#define CONCENTRATION_H

#include "statistic.h"

class Concentration : public Statistic
{
    Q_OBJECT
    Q_PROPERTY(QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged)
public:
    explicit Concentration(QObject *parent = nullptr);
    virtual void compute(Geometry *geometry) override;
    QString fileName() const;

public slots:
    void setFileName(QString fileName);

signals:
    void fileNameChanged(QString fileName);

private:
    bool isValid();
    QVector<float> m_pressures;
    QVector<QVector<float>> m_values;
    QVector<float> m_volumes;
    void computeMode0(Geometry *geometry);
    void computeMode1(Geometry *geometry);
    QString m_fileName;
    void readFile();
};

#endif // CONCENTRATION_H
