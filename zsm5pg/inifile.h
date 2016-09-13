#ifndef INIFILE_H
#define INIFILE_H
#include <QString>
#include <QMap>
#include <QObject>

class IniFile : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString filename READ filename WRITE setFilename NOTIFY filenameChanged)
    Q_PROPERTY(bool ready READ ready WRITE setReady NOTIFY readyChanged)
public:
    IniFile(QObject *parent = nullptr);
    QMap<QString, QString> m_keyValuePairs;
    QString getString(QString key);
    double getDouble(QString key);
    int getInt(QString key);
    bool getBool(QString key);
    QVector<double> getDoubleArray(QString key);
    QVector<int> getIntArray(QString key);
    bool hasKey(QString key);
    bool contains(QString key);
    QString filename();
    bool ready() const;

public slots:
    void setFilename(QString filename);
    void setReady(bool ready);

signals:
    void filenameChanged(QString filename);
    void readyChanged(bool ready);

private:
    QString m_filename;
    bool m_ready = false;
    void loadFile();
};

#endif // INIFILE_H
