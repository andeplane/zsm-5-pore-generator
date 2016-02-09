#ifndef ZSM5GEOMETRY_H
#define ZSM5GEOMETRY_H
#include <QObject>
#include <QVector>
class Zsm5geometry : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int planesPerDimension READ planesPerDimension WRITE setPlanesPerDimension NOTIFY planesPerDimensionChanged)
    Q_PROPERTY(float lengthScale READ lengthScale WRITE setLengthScale NOTIFY lengthScaleChanged)
    Q_PROPERTY(bool dirty READ dirty WRITE setDirty NOTIFY dirtyChanged)
private:
    QVector<float> m_deltaXVector;
    QVector<float> m_deltaYVector;
    QVector<float> m_deltaZVector;
    int m_planesPerDimension = 10;
    float m_lengthScale = 5;
    bool m_dirty = false;

public:
    Zsm5geometry();
    ~Zsm5geometry();
    void reset();
    void randomWalkStep(float standardDeviation);
    Q_INVOKABLE void save(QString filename);
    Q_INVOKABLE void load(QString filename);
    QVector<float> &deltaXVector() { return m_deltaXVector; }
    QVector<float> &deltaYVector() { return m_deltaYVector; }
    QVector<float> &deltaZVector() { return m_deltaZVector; }

    void setDeltaXVector(const QVector<float> &deltaX) { m_deltaXVector = deltaX; m_dirty = true; }
    void setDeltaYVector(const QVector<float> &deltaY) { m_deltaYVector = deltaY; m_dirty = true; }
    void setDeltaZVector(const QVector<float> &deltaZ) { m_deltaZVector = deltaZ; m_dirty = true; }

    int planesPerDimension() const;
    void setPlanesPerDimension(int planesPerDimension);
    float lengthScale() const;
    bool dirty() const;

public slots:
    void setLengthScale(float lengthScale);
    void setDirty(bool dirty);

signals:
    void planesPerDimensionChanged(int planesPerDimension);
    void lengthScaleChanged(float lengthScale);
    void dirtyChanged(bool dirty);
};

#endif // ZSM5GEOMETRY_H
