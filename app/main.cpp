#include "dplayer.h"

#ifdef Q_OS_WIN
QString separator = QDir::toNativeSeparators(QDir::separator());
#else
QString separator = QDir::toNativeSeparators(QDir::separator());
#endif

QString pathDBfile;
QSqlDatabase db;
QQmlContext *mainContext;

int main(int argc, char * argv[]){
    /* первым делом создадим директорию для работы нашей проги */
    pathDBfile = QDir::toNativeSeparators(QDir::homePath()) + separator + ".dplayer";

    if (!QDir(pathDBfile).exists()) {
        qDebug() << "path programm not exists! Create...";
        QDir().mkdir(pathDBfile);
    }

    /* вторым делом подготовим таблицы */
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(pathDBfile + separator + "dbplayer.db");
    if (!db.open()) {
        QString message = db.lastError().text();
        qDebug() << "DB error: " << message;
    }
    QSqlQuery a_query(db);
    QString str = "CREATE TABLE IF NOT EXISTS file ("
        "id integer PRIMARY KEY AUTOINCREMENT NOT NULL, "
        "name VARCHAR(255), "
        "create_date DATETIME"
        ");";
    bool b = a_query.exec(str);
    if (!b) {
        QString message = db.lastError().text();
        qDebug() << "DB Create table [file] error: " << message;
    }
    str = "CREATE TABLE IF NOT EXISTS conf ("
        "name VARCHAR(255) PRIMARY KEY NOT NULL, "
        "value VARCHAR(255) "
        ");";
    b = a_query.exec(str);
    if (!b) {
        QString message = db.lastError().text();
        qDebug() << "DB Create table [file] error: " << message;
    }
    db.close();     // закроем подключение к базе
    /* конец создания таблиц */

    /* инициализируем гуи */
    QGuiApplication app(argc, argv);
    QQmlEngine engine;
    mainContext = engine.rootContext();
    QQmlComponent component(&engine, QUrl(QStringLiteral("qrc:/main.qml")));

    /* маппим объекты GML с нашими классами */
    QObject * object = component.create();
    watcherTxtDrive chekUSB;
    chekUSB.txtDrive = object->findChild<QObject *>("txtDrive");
    chekUSB.txtMemo = object->findChild<QObject *>("txtMemo");

    /* нужно просканировать наличие УЖЕ подключенных съемных дисков WIN32*/
    QFileInfoList drivers = QDir::drives();
    foreach(QFileInfo drive, drivers) {
        chekUSB.slotDeviceAdded(drive.absoluteFilePath());
    }
    /* просканировали... */

    return app.exec();
} // main


watcherTxtDrive::watcherTxtDrive(QQmlEngine * parent) :
    QQmlEngine(parent){
    watcher = new QDeviceWatcher;
    watcher->appendEventReceiver(this);
    connect(watcher, SIGNAL(deviceAdded(QString)), this, SLOT(slotDeviceAdded(QString)), Qt::DirectConnection);
    connect(watcher, SIGNAL(deviceRemoved(QString)), this, SLOT(slotDeviceRemoved(QString)), Qt::DirectConnection);
    watcher->start();
}


void watcherTxtDrive::slotDeviceAdded(const QString &dev){
    if (DRIVE_REMOVABLE == GetDriveType((wchar_t *)dev.utf16())) {
        QString pathDEVfile = QDir::toNativeSeparators(dev) + ".devinfo";
        if (QFileInfo(pathDEVfile).exists()) {
            if (txtDrive)
                txtDrive->setProperty("text", QDir::toNativeSeparators(dev));
            qDebug() << "detect VIDEOREGISTRATOR in drive " << QDir::toNativeSeparators(dev);

            // запускаем доп.поток для копирования в фоне от основного процесса
            QThread * thread = new QThread;
            Worker * worker = new Worker(dev);
            worker->moveToThread(thread);
            connect(worker, SIGNAL(msg(QString)), this, SLOT(msg(QString)));
            connect(thread, SIGNAL(started()), worker, SLOT(process()));
            connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
            connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
            connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
            thread->start();
        }
    }
} // watcherTxtDrive::slotDeviceAdded

void watcherTxtDrive::slotDeviceRemoved(const QString &dev){
    if (txtDrive)
        txtDrive->setProperty("text", "no usb disk");
}

void watcherTxtDrive::msg(QString msg){
    if (txtMemo) {
        txtMemo->setProperty("text", msg);

        QList<QObject*> dataList;

        if (!db.open()) {
            QString message = db.lastError().text();
            qDebug() << "DB error: " << message;
        }
        QSqlQuery query("SELECT id, name, create_date FROM file");
        while (query.next()) {
            dataList.append(new TableFile(query.value(0).toInt(), query.value(1).toString(), query.value(2).toUInt()));
        }
        mainContext->setContextProperty("myModel", QVariant::fromValue(dataList));
    }
}

// --- CONSTRUCTOR ---
Worker::Worker(const QString& devinit){
    // you could copy data from constructor arguments to internal variables here.
    dev = devinit;
}

// --- DECONSTRUCTOR ---
Worker::~Worker(){
    // free resources
    qDebug() << "Thread for sync " << QDir::toNativeSeparators(dev) << " finished.";
}

// --- PROCESS ---
// Start processing data.
void Worker::process(){
    // allocate resources using new here

    if (!db.open()) {
        QString message = db.lastError().text();
        qDebug() << "DB error: " << message;
    }

    /* сканируем файлы на флэшке */

    QString srcDirMove = QDir::toNativeSeparators(dev) + "DCIM";
    QStringList listDir = QDir(srcDirMove, "*VIDEO", QDir::Name | QDir::IgnoreCase, QDir::Dirs | QDir::NoDotAndDotDot).entryList();
    foreach(QString subdir, listDir) {
        qDebug() << srcDirMove + separator + subdir;
        QFileInfoList listFile = QDir(srcDirMove + separator + subdir, "*.MP4", QDir::Name | QDir::IgnoreCase, QDir::Files | QDir::NoDotAndDotDot).entryInfoList();
        foreach(QFileInfo subfile, listFile) {
            qDebug() << subfile.created().toString("yyyy.MM.dd HH:mm:ss") + " | " + subfile.fileName();
            QString month = subfile.created().toString("MM");
            QString year = subfile.created().toString("yyyy");

            QSqlQuery a_query(db);
            int result = 0;

            // Проверяем, есть ли уже у нас в базе такой файл? если нету - копируем к себе
            if (!a_query.exec("select count(1) result from file where name = '" + year + separator + month + separator + subfile.created().toString("yyyyMMddHHmmss") + "_" + subfile.fileName() + "'")) {
                QString message = db.lastError().text();
                qDebug() << "DB find file error: " << message;
            }
            if (a_query.next()) {
                QSqlRecord rec = a_query.record();
                result = a_query.value(rec.indexOf("result")).toInt();
            }

            if (!result) {
                if (!QDir(pathDBfile + separator + year).exists()) {
                    qDebug() << "year folder " + year + " create...";
                    QDir().mkdir(pathDBfile + separator + year);
                }
                if (!QDir(pathDBfile + separator + year + separator + month).exists()) {
                    qDebug() << "month folder " + month + " in " + year + " year folder create...";
                    QDir().mkdir(pathDBfile + separator + year + separator + month);
                }

                emit msg("Copy file [" + subfile.fileName() + "] in process...");

                QString toFile = year + separator + month + separator + subfile.created().toString("yyyyMMddHHmmss") + "_" + subfile.fileName();

                // В базе нету, а на диске есть? прошлое копирование прошло не так гладко... удалим косячной файл
                if (QFile::exists(pathDBfile + separator + toFile))
                    QFile::remove(pathDBfile + separator + toFile);

                if (QFile::copy(subfile.absoluteFilePath(), pathDBfile + separator + toFile)) {
                    a_query.prepare("INSERT INTO file (name, create_date)"
                                    "VALUES (:name, :create_date);");
                    a_query.bindValue(":name", toFile);
                    a_query.bindValue(":create_date", subfile.created().toTime_t());
                    a_query.exec();
                } else {
                    qDebug() << "file copy " + subfile.absoluteFilePath() + " failed";
                }
            }
        }
    }
    /* просканировали */
    emit msg("Sync files from device complete.");
    db.close();

    emit finished();
} // Worker::process

TableFile::TableFile(const int &id, const QString &name, const uint &utime)
    : m_id(id), m_name(name), m_utime(utime)
{
}

int TableFile::id() const
{
    return m_id;
}

QString TableFile::name() const
{
    return m_name;
}

uint TableFile::utime() const
{
    return m_utime;
}
