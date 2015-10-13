#ifndef DPLAYER
#define DPLAYER

#include <QGuiApplication>
#include <QQmlEngine>

#include <QQmlComponent>
#include <QString>

#include <QStringList>
#include <QQmlContext>

#include <QtCore/QObject>
#include <QtCore/QThread>
#include "qdevicewatcher.h"

#include <QtSql>
#include <QDir>

#ifndef __GNUC__
#define __PRETTY_FUNCTION__ __FUNCTION__
#endif

#include <qt_windows.h>

// Класс для событий УСБ-устройств
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

// Класс для потоков
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

// Класс для передачи модели в QML-ный ListView
class TableFile : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int id READ id)
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(uint utime READ utime)

public:
    TableFile(const int &id, const QString &name, const uint &utime);

    int id() const;
    QString name() const;
    uint utime() const;

signals:
    void nameChanged();

private:
    int m_id;
    QString m_name;
    uint m_utime;
};

#endif // DPLAYER
