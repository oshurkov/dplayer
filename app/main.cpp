/* test */

#include "dplayer.h"

#ifdef Q_OS_WIN
    QString separator = QDir::toNativeSeparators(QDir::separator());
#else
    QString separator = QDir::toNativeSeparators(QDir::separator());
#endif

    QString pathDBfile;
    QSqlDatabase db;

int main(int argc, char *argv[])
{

    QGuiApplication app(argc, argv);

    QQmlEngine engine;

    QQmlComponent component(&engine, QUrl(QStringLiteral("qrc:/main.qml")));
    QObject *object = component.create();

    watcherTxtDrive chekUSB;
    chekUSB.txtDrive = object->findChild<QObject*>("txtDrive");

/* работа с SQLite и файлом-конфига */
    pathDBfile = QDir::toNativeSeparators(QDir::homePath()) + separator + ".dplayer";

    if (!QDir(pathDBfile).exists()){
        qDebug() << "path programm not exists! Create...";
        QDir().mkdir(pathDBfile);
    }

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(pathDBfile + separator + "dbplayer.db");

    if ( !db.open() )
    {
        QString message = db.lastError().text();
        qDebug() << "DB error: " << message;
    }

    QSqlQuery a_query(db);
    int result = 0;

    //Создаем таблицы, если уже они существуют - пропускаем этот шаг
    if (!a_query.exec("select count(1) result from sqlite_master where type = 'table' and name in ('file','conf')")) {
        QString message = db.lastError().text();
        qDebug() << "DB check tables error: " << message;
    }
    if (a_query.next()) {
        QSqlRecord rec = a_query.record();
        result = a_query.value(rec.indexOf("result")).toInt();
    }

    if (!result) {
        // DDL...
        QString str = "CREATE TABLE file ("
                "id integer PRIMARY KEY AUTOINCREMENT NOT NULL, "
                "name VARCHAR(255), "
             "create_date DATETIME"
             ");";
        bool b = a_query.exec(str);
        if (!b) {
            QString message = db.lastError().text();
            qDebug() << "DB Create table [file] error: " << message;
        }

        str = "CREATE TABLE conf ("
                "name VARCHAR(255) PRIMARY KEY NOT NULL, "
                "value VARCHAR(255) "
             ");";
        b = a_query.exec(str);
        if (!b) {
            QString message = db.lastError().text();
            qDebug() << "DB Create table [file] error: " << message;
        }
    }
/* конец SQLite и файла конфига */

/* нужно просканировать наличие УЖЕ подключенных съемных дисков WIN32*/
    QFileInfoList drivers = QDir::drives();
    foreach (QFileInfo drive, drivers) {
        chekUSB.slotDeviceAdded(drive.absoluteFilePath());
    }
/* просканировали... */

    db.close(); //закроем подключение к базе
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
    if (DRIVE_REMOVABLE == GetDriveType((wchar_t *)dev.utf16())) {
        QString pathDEVfile = QDir::toNativeSeparators(dev) + ".devinfo";
        if (QFileInfo(pathDEVfile).exists()){
            if (txtDrive)
                txtDrive->setProperty("text", QDir::toNativeSeparators(dev));
            qDebug() << "detect VIDEOREGISTRATOR in drive " << QDir::toNativeSeparators(dev);
            /* сканируем файлы на флэшке */
                //if (chekUSB.txtDrive->property("text").toString() != "no usb disk") {
                    QString srcDirMove = QDir::toNativeSeparators(dev) + "DCIM";
                    QStringList listDir = QDir(srcDirMove,"*VIDEO",QDir::Name | QDir::IgnoreCase,QDir::Dirs | QDir::NoDotAndDotDot).entryList();
                    foreach (QString subdir, listDir) {
                        qDebug() << srcDirMove + separator + subdir;
                        QFileInfoList listFile = QDir(srcDirMove + separator + subdir,"*.MP4",QDir::Name | QDir::IgnoreCase,QDir::Files | QDir::NoDotAndDotDot).entryInfoList();
                        foreach (QFileInfo subfile, listFile) {
                            qDebug() << subfile.created().toString("yyyy.MM.dd HH:mm:ss") + " | " + subfile.fileName();
                            QString month = subfile.created().toString("MM");
                            QString year = subfile.created().toString("yyyy");

                            QSqlQuery a_query(db);
                            int result = 0;

                            //Создаем таблицы, если уже они существуют - пропускаем этот шаг
                            if (!a_query.exec("select count(1) result from file where name = '" + year + separator + month + separator + subfile.created().toString("yyyyMMddHHmmss") + "_" + subfile.fileName() + "'")) {
                                QString message = db.lastError().text();
                                qDebug() << "DB find file error: " << message;
                            }
                            if (a_query.next()) {
                                QSqlRecord rec = a_query.record();
                                result = a_query.value(rec.indexOf("result")).toInt();
                            }

                            if (!result) {

                            if (!QDir(pathDBfile + separator + year).exists()){
                                qDebug() << "year folder " + year + " create...";
                                QDir().mkdir(pathDBfile + separator + year);
                            }
                            if (!QDir(pathDBfile + separator + year + separator + month).exists()){
                                qDebug() << "month folder " + month + " in " + year + " year folder create...";
                                QDir().mkdir(pathDBfile + separator + year + separator + month);
                            }
                            if (QFile::copy(subfile.absoluteFilePath(), pathDBfile + separator + year + separator + month + separator + subfile.created().toString("yyyyMMddHHmmss") + "_" + subfile.fileName() )) {
                                a_query.prepare("INSERT INTO file (name, create_date)"
                                                              "VALUES (:name, :create_date);");
                                a_query.bindValue(":name", year + separator + month + separator + subfile.created().toString("yyyyMMddHHmmss") + "_" + subfile.fileName());
                                a_query.bindValue(":create_date", subfile.created().toTime_t());
                                a_query.exec();
                            } else {
                                qDebug() << "file copy " + subfile.absoluteFilePath() + " failed";
                            }

                            }
                        }
                    }
                //}
            /* просканировали */
        }
    }
}

void watcherTxtDrive::slotDeviceRemoved(const QString &dev)
{
    if (txtDrive)
        txtDrive->setProperty("text", "no usb disk");
}

