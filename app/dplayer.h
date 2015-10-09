#ifndef DPLAYER
#define DPLAYER

#include <QGuiApplication>
#include <QQmlEngine>

#include <QQmlComponent>
#include <QString>

#include <QtCore/QObject>
#include <QtCore/QThread>
#include "qdevicewatcher.h"

#include <QtSql>
#include <QDir>

#ifndef __GNUC__
#define __PRETTY_FUNCTION__  __FUNCTION__
#endif

#include <qt_windows.h>

class watcherTxtDrive : public QQmlEngine
{
    Q_OBJECT
public:
    explicit watcherTxtDrive(QQmlEngine *parent = 0);
    QObject *txtDrive;
    QObject *txtMemo;

public slots:
   void slotDeviceAdded(const QString& dev);
   void slotDeviceRemoved(const QString& dev);
   void msg(QString msg);

private:
    QDeviceWatcher *watcher;

};

class Worker : public QObject {
    Q_OBJECT

public:
    Worker(const QString& devinit);
    virtual ~Worker();

public slots:
    void process();

signals:
    void finished();
    void msg(QString msg);

private:
    QString dev;
};

#endif // DPLAYER

