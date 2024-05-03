#pragma once

#include <QObject>
#include <QString>
#include <QDebug>
#include <QFileInfoList>
#include <QUrl>
#include "FileCopyer.h"

class DataVM : public QObject
{
	Q_OBJECT

public:
    DataVM(QObject* parent = nullptr);
    static DataVM* getInstance();

    Q_INVOKABLE void copyFiles(QFileInfoList filePaths, QString toFilePath);
    Q_INVOKABLE void interruptCopyFile();

signals:
    void sigCopyFileProgress(float progress);
    void sigOneCopyFileBegin(QString fileName);
    void sigAllCopyFilesFinished(bool s);
    void copyonesuccess(QString srcPath,QString uuidstr);
private:

    ~DataVM();
    static DataVM* m_instance;
    std::unique_ptr<FileCopyer>   m_worker;
};
