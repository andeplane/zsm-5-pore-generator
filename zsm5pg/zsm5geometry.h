#ifndef ZSM5GEOMETRY_H
#define ZSM5GEOMETRY_H
#include <QObject>
#include <QVector>
#include <vector>
using std::vector;
class Zsm5geometry : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int planesPerDimension READ planesPerDimension WRITE setPlanesPerDimension NOTIFY planesPerDimensionChanged)
    Q_PROPERTY(float lengthScale READ lengthScale WRITE setLengthScale NOTIFY lengthScaleChanged)
private:
    vector<float> m_deltaXVector;
    vector<float> m_deltaYVector;
    vector<float> m_deltaZVector;
    int m_planesPerDimension = 10;
    float m_lengthScale = 5;

public:
    Zsm5geometry();
    ~Zsm5geometry();
    void reset();
    void randomWalkStep(float standardDeviation);

    vector<float> &deltaXVector() { return m_deltaXVector; }
    vector<float> &deltaYVector() { return m_deltaYVector; }
    vector<float> &deltaZVector() { return m_deltaZVector; }

    int planesPerDimension() const;
    void setPlanesPerDimension(int planesPerDimension);
    float lengthScale() const;

public slots:
    void setLengthScale(float lengthScale);

signals:
    void planesPerDimensionChanged(int planesPerDimension);
    void lengthScaleChanged(float lengthScale);
};

#endif // ZSM5GEOMETRY_H
