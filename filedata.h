#ifndef FILEDATA_H
#define FILEDATA_H

#include <QFile>

class FileData
{
public:
    FileData(QString);

private:
    QString fname;

public:
    void writeAppend(QString);
    QString readLine(int);
    int getLine(QString);
    void deleteLine(int);
    void deleteByCode(QString);
    QString searchByCode(QString);
    void EditLine(int, QString);
    void EditByCode(QString, QString);
    int length();
};

#endif // FILEDATA_H
