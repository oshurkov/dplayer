/* test */

#include "dplayer.h"

#ifdef WIN32
    QString separator = QDir::toNativeSeparators(QDir::separator());
#else
    QString separator = QDir::toNativeSeparators(QDir::separator());
#endif

int main(int argc, char *argv[])
{

    QGuiApplication app(argc, argv);

    QQmlEngine engine;

    QQmlComponent component(&engine, QUrl(QStringLiteral("qrc:/main.qml")));
    QObject *object = component.create();

    watcherTxtDrive chekUSB;
    chekUSB.txtDrive = object->findChild<QObject*>("txtDrive");



    QString pathDBfile = QDir::toNativeSeparators(QDir::homePath()) + separator + ".dplayer";
    qDebug() << pathDBfile;

    if (!QDir(pathDBfile).exists()){
        qDebug() << "path programm not exists! Create...";
        QDir().mkdir(pathDBfile);
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(pathDBfile + separator + "dbplayer.db");

    if ( !db.open() )
    {
        QString message = db.lastError().text();
        qDebug() << "DB error: " << message;
    }

    db.close();

    return app.exec();
}


watcherTxtDrive::watcherTxtDrive(QQmlEngine *parent) :
     QQmlEngine(parent) {

     watcher = new QDeviceWatcher;
     watcher->appendEventReceiver(this);
     connect(watcher, SIGNAL(deviceAdded(QString)), this, SLOT(slotDeviceAdded(QString)), Qt::DirectConnection);
     connect(watcher, SIGNAL(deviceRemoved(QString)), this, SLOT(slotDeviceRemoved(QString)), Qt::DirectConnection);
     watcher->start();
 }


void watcherTxtDrive::slotDeviceAdded(const QString &dev)
{
    if ( txtDrive)
        txtDrive->setProperty("text", dev);

}

void watcherTxtDrive::slotDeviceRemoved(const QString &dev)
{
    if ( txtDrive)
        txtDrive->setProperty("text", "no usb disk");
}

