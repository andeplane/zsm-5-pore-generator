#ifndef MONTECARLO_H
#define MONTECARLO_H
#include <QFile>
#include <QObject>
#include <fstream>
#include "zsm5geometry.h"
#include "statistics/statistics.h"

class MonteCarlo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Geometry* geometry READ geometry WRITE setGeometry NOTIFY geometryChanged)
    Q_PROPERTY(QVariantList models READ models WRITE setModels NOTIFY modelsChanged)
    Q_PROPERTY(QVariantList datas READ datas WRITE setDatas NOTIFY datasChanged)
    Q_PROPERTY(float standardDeviation READ standardDeviation WRITE setStandardDeviation NOTIFY standardDeviationChanged)
    Q_PROPERTY(float temperature READ temperature WRITE setTemperature NOTIFY temperatureChanged)
    Q_PROPERTY(int steps READ steps WRITE setSteps NOTIFY stepsChanged)
    Q_PROPERTY(int accepted READ accepted WRITE setAccepted NOTIFY acceptedChanged)
    Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)
    Q_PROPERTY(float chiSquared READ chiSquared WRITE setChiSquared NOTIFY chiSquaredChanged)
    Q_PROPERTY(float targetAcceptanceRatio READ targetAcceptanceRatio WRITE setTargetAcceptanceRatio NOTIFY targetAcceptanceRatioChanged)
    Q_PROPERTY(float acceptanceRatio READ acceptanceRatio WRITE setAcceptanceRatio NOTIFY acceptanceRatioChanged)
    Q_PROPERTY(float acceptanceRatioAdjustmentTimeScale READ acceptanceRatioAdjustmentTimeScale WRITE setAcceptanceRatioAdjustmentTimeScale NOTIFY acceptanceRatioAdjustmentTimeScaleChanged)
    Q_PROPERTY(QString filename READ filename WRITE setFilename NOTIFY filenameChanged)
    Q_PROPERTY(QString filePath READ filePath WRITE setFilePath NOTIFY filePathChanged)
private:
    QFile m_file;
    Geometry* m_geometry = nullptr;
    float m_acceptanceRatio = 1;
    float m_targetAcceptanceRatio = 0.7;
    float m_standardDeviation = 1.0;
    float m_chiSquared = 0.0;
    float m_temperature = 1.0;
    int m_steps = 0;
    int m_accepted = 0;
    bool m_running = false;
    bool m_debug = false;
    QVariantList m_models;
    QVariantList m_datas;
    QString m_filename;
    float m_acceptanceRatioAdjustmentTimeScale = 0.1;
    void updateRandomWalkFraction();
    void writeToFile();
    QString m_filePath;

public:
    MonteCarlo();
    Geometry* geometry() const;
    void tick();
    void loadInifile(class IniFile *iniFile);
    float standardDeviation() const;
    float temperature() const;
    int steps() const;
    int accepted() const;
    bool running() const;
    bool isValid() const;
    float acceptanceRatio();
    float chiSquared() const;
    void setDebug(bool debug);
    float targetAcceptanceRatio() const;
    float acceptanceRatioAdjustmentTimeScale() const;
    QString filename() const;
    QVariantList models() const;
    QVariantList datas() const;
    QString filePath() const;

public slots:
    void setGeometry(Geometry* geometry);
    void setStandardDeviation(float standardDeviation);
    void setTemperature(float temperature);
    void setSteps(int steps);
    void setAccepted(int accepted);
    void setRunning(bool running);
    void setChiSquared(float chiSquared);
    void setTargetAcceptanceRatio(float targetAcceptanceRatio);
    void setAcceptanceRatio(float acceptanceRatio);
    void setAcceptanceRatioAdjustmentTimeScale(float acceptanceRatioAdjustmentTimeScale);
    void setFilename(QString filename);
    void setModels(QVariantList models);
    void setDatas(QVariantList datas);
    void setFilePath(QString filePath);

signals:
    void geometryChanged(Geometry* geometry);
    void standardDeviationChanged(float standardDeviation);
    void temperatureChanged(float temperature);
    void stepsChanged(int steps);
    void acceptedChanged(int accepted);
    void runningChanged(bool running);
    void chiSquaredChanged(float chiSquared);
    void targetAcceptanceRatioChanged(float targetAcceptanceRatio);
    void acceptanceRatioChanged(float acceptanceRatio);
    void acceptanceRatioAdjustmentTimeScaleChanged(float acceptanceRatioAdjustmentTimeScale);
    void filenameChanged(QString filename);
    void modelsChanged(QVariantList models);
    void datasChanged(QVariantList datas);
    void filePathChanged(QString filePath);
};

#endif // MONTECARLO_H
