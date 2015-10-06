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

/* нужно просканировать наличие УЖЕ подключенных съемных дисков WIN32*/
    QFileInfoList drivers = QDir::drives();
    QFileInfo videoDrive; //наш диск, с которым будем работать
    foreach (QFileInfo drive, drivers) {
        if (DRIVE_REMOVABLE == GetDriveType((wchar_t *)drive.absoluteFilePath().utf16())) {
            QString pathDEVfile = QDir::toNativeSeparators(drive.absoluteFilePath()) + ".devinfo";
            if (QFileInfo(pathDEVfile).exists()){
                videoDrive = drive;
                chekUSB.slotDeviceAdded( QDir::toNativeSeparators(videoDrive.absoluteFilePath()));
                qDebug() << "detect VIDEOREGISTRATOR in drive " << QDir::toNativeSeparators(drive.absoluteFilePath());
            }
        }
    }
    if (!videoDrive.exists()) {
        qDebug() << "removable drive not found";
    }
/* просканировали... */


/* работа с SQLite и файлом-конфига */
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
/* конец SQLite и файла конфига */

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
    if (txtDrive)
        txtDrive->setProperty("text", dev);

}

void watcherTxtDrive::slotDeviceRemoved(const QString &dev)
{
    if (txtDrive)
        txtDrive->setProperty("text", "no usb disk");
}

