#ifndef DPLAYER
#define DPLAYER

#include <QGuiApplication>
#include <QQmlEngine>

#include <QQmlComponent>
#include <QString>

#include <QtCore/QObject>
#include <QtCore/QThread>
#include "qdevicewatcher.h"

#ifndef __GNUC__
#define __PRETTY_FUNCTION__  __FUNCTION__
#endif



class watcherTxtDrive : public QQmlEngine
{
    Q_OBJECT
public:
    explicit watcherTxtDrive(QQmlEngine *parent = 0);
    QObject *txtDrive;

public slots:
   void slotDeviceAdded(const QString& dev);
   void slotDeviceRemoved(const QString& dev);


private:
    QDeviceWatcher *watcher;

};

#endif // DPLAYER

