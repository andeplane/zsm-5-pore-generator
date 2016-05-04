#include "inifile.h"
#include <QFile>
#include <QUrl>
#include <QDebug>

IniFile::IniFile(QString filename) :
    m_filename(filename)
{
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Could not open " << filename;
        exit(1);
    }

    QString content(file.readAll());
    QStringList lines = content.split("\n");
    for(QString line : lines) {
        line = line.trimmed();
        if(line.startsWith("#")) continue; // skip comment lines
        if(!line.contains("=")) continue; // We need something = yeahbuddy
        QStringList splittedLine = line.split("=");
        QString key = splittedLine[0].trimmed();
        QString value = splittedLine[1].trimmed();
        if(key.split(" ").count() > 1) {
            qDebug() << "Error, key " << key << " contains whitespaces.";
            exit(1);
        }
        m_keyValuePairs.insert(key, value);
    }

    file.close();
}

QString IniFile::getString(QString key)
{
    if(m_keyValuePairs.contains(key)) {
        return m_keyValuePairs[key];
    } else return QString("");
}

double IniFile::getDouble(QString key)
{
    if(m_keyValuePairs.contains(key)) {
        QString valueString = m_keyValuePairs[key];
        bool ok;
        double value = valueString.toDouble(&ok);
        if(ok) return value;
        else {
            qDebug() << "Error, could not parse double for key " << key;
            exit(1);
        }
    }
    qDebug() << "Error, could not find key " << key;
    exit(1);
}

int IniFile::getInt(QString key)
{
    if(m_keyValuePairs.contains(key)) {
        QString valueString = m_keyValuePairs[key];
        bool ok;
        int value = valueString.toInt(&ok);
        if(ok) return value;
        else {
            qDebug() << "Error, could not parse int for key " << key;
            exit(1);
        }
    }
    qDebug() << "Error, could not find key " << key;
    exit(1);
}

bool IniFile::getBool(QString key)
{
    if(m_keyValuePairs.contains(key)) {
        QString value = m_keyValuePairs[key];
        return (value.toLower().compare("true") == 0);
    }

    qDebug() << "Error, could not find key " << key;
    exit(1);
}

QVector<double> IniFile::getDoubleArray(QString key)
{
    if(m_keyValuePairs.contains(key)) {
        QString valueString = m_keyValuePairs[key];
        QStringList valueStringSplitted = valueString.split(" ");
        QVector<double> values;
        for(QString v : valueStringSplitted) {
            bool ok;
            double value = v.toDouble(&ok);
            if(ok) values.push_back(value);
        }
        return values;
    }
    qDebug() << "Error, could not find key " << key;
    exit(1);
}

QVector<int> IniFile::getIntArray(QString key)
{
    if(m_keyValuePairs.contains(key)) {
        QString valueString = m_keyValuePairs[key];
        QStringList valueStringSplitted = valueString.split(" ");
        QVector<int> values;
        for(QString v : valueStringSplitted) {
            bool ok;
            int value = v.toInt(&ok);
            if(ok) values.push_back(value);
        }
        return values;
    }
    qDebug() << "Error, could not find key " << key;
    exit(1);
}

bool IniFile::contains(QString key) {
    return m_keyValuePairs.contains(key);
}

bool IniFile::hasKey(QString key)
{
    return contains(key);
}

