#ifndef GEOMETRY_H
#define GEOMETRY_H
#include <QObject>
#include <QVector>
class Geometry : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int planesPerDimension READ planesPerDimension WRITE setPlanesPerDimension NOTIFY planesPerDimensionChanged)
    Q_PROPERTY(float lengthScale READ lengthScale WRITE setLengthScale NOTIFY lengthScaleChanged)
    Q_PROPERTY(float randomWalkFraction READ randomWalkFraction WRITE setRandomWalkFraction NOTIFY randomWalkFractionChanged)
    Q_PROPERTY(int mode READ mode WRITE setMode NOTIFY modeChanged)
    Q_PROPERTY(QString filePath READ filePath WRITE setFilePath NOTIFY filePathChanged)
    Q_PROPERTY(bool isValid READ isValid WRITE setIsValid NOTIFY isValidChanged)
private:
    QVector<float> m_deltaXVector;
    QVector<float> m_deltaYVector;
    QVector<float> m_deltaZVector;
    int m_planesPerDimension = 10;
    float m_lengthScale = 5;
    float m_randomWalkFraction = 1.0;
    int m_mode = 0;
    QString m_filePath;
    bool m_isValid = false;

public:
    Geometry();
    ~Geometry();
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
    int mode() const;
    QString filePath() const;
    bool isValid() const;

public slots:
    void setLengthScale(float lengthScale);
    void setRandomWalkFraction(float randomWalkFraction);
    void setMode(int mode);
    void setFilePath(QString filePath);
    void setIsValid(bool isValid);
    void loadIniFile(class IniFile *iniFile);

signals:
    void planesPerDimensionChanged(int planesPerDimension);
    void lengthScaleChanged(float lengthScale);
    void randomWalkFractionChanged(float randomWalkFraction);
    void modeChanged(int mode);
    void filePathChanged(QString filePath);
    void isValidChanged(bool isValid);
};

#endif // GEOMETRY_H
