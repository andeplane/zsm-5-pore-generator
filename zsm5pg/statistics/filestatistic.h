#ifndef FILESTATISTIC_H
#define FILESTATISTIC_H

#include "statistic.h"



class FileStatistic : public Statistic
{
    Q_OBJECT
    Q_PROPERTY(QString filename READ filename WRITE setFilename NOTIFY filenameChanged)
    QString m_filename;
    bool m_dirty = true;
public:
    FileStatistic();
    QString filename() const;

    // Statistic interface
public:
    virtual void compute();

public slots:
    void setFilename(QString filename);

signals:
    void filenameChanged(QString filename);
};

#endif // FILESTATISTIC_H
