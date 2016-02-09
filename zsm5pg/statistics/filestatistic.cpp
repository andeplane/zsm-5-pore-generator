#include "filestatistic.h"
#include <QFile>

FileStatistic::FileStatistic()
{

}


void FileStatistic::compute()
{
    if(!m_dirty) return;
    m_xValuesRaw.resize(m_bins);
    m_yValuesRaw.resize(m_bins);

}

QString FileStatistic::filename() const
{
    return m_filename;
}

void FileStatistic::setFilename(QString filename)
{
    if (m_filename == filename)
        return;

    m_filename = filename;
    m_dirty = true;
    emit filenameChanged(filename);
}
