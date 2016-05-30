#ifndef INIFILE_H
#define INIFILE_H
#include <QString>
#include <QMap>

class IniFile
{
public:
    IniFile(QString filename);
    QMap<QString, QString> m_keyValuePairs;
    QString getString(QString key);
    double getDouble(QString key);
    int getInt(QString key);
    bool getBool(QString key);
    QVector<double> getDoubleArray(QString key);
    QVector<int> getIntArray(QString key);
    bool hasKey(QString key);
    bool contains(QString key);
    QString filename() { return m_filename; }
private:
    QString m_filename;
};

#endif // INIFILE_H
