#ifndef ZSM5GEOMETRY_H
#define ZSM5GEOMETRY_H
#include <QObject>
#include <QVector>
class Zsm5geometry : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int planesPerDimension READ planesPerDimension WRITE setPlanesPerDimension NOTIFY planesPerDimensionChanged)
    Q_PROPERTY(float lengthScale READ lengthScale WRITE setLengthScale NOTIFY lengthScaleChanged)
    Q_PROPERTY(float randomWalkFraction READ randomWalkFraction WRITE setRandomWalkFraction NOTIFY randomWalkFractionChanged)
private:
    QVector<float> m_deltaXVector;
    QVector<float> m_deltaYVector;
    QVector<float> m_deltaZVector;
    int m_planesPerDimension = 10;
    float m_lengthScale = 5;
    float m_randomWalkFraction = 1.0;

public:
    Zsm5geometry();
    ~Zsm5geometry();
    void reset(float min, float max);
    void randomWalkStep(float standardDeviation);
    Q_INVOKABLE void save(QString filename);
    Q_INVOKABLE void load(QString filename);
    QVector<float> &deltaXVector();
    QVector<float> &deltaYVector();
    QVector<float> &deltaZVector();

    void setDeltaXVector(const QVector<float> &deltaX);
    void setDeltaYVector(const QVector<float> &deltaY);
    void setDeltaZVector(const QVector<float> &deltaZ);

    int planesPerDimension() const;
    void setPlanesPerDimension(int planesPerDimension);
    float lengthScale() const;
    double totalVolume();
    void resize(int newNumberOfPlanes);

    float randomWalkFraction() const;

public slots:
    void setLengthScale(float lengthScale);

    void setRandomWalkFraction(float randomWalkFraction);

signals:
    void planesPerDimensionChanged(int planesPerDimension);
    void lengthScaleChanged(float lengthScale);
    void randomWalkFractionChanged(float randomWalkFraction);
};

#endif // ZSM5GEOMETRY_H
