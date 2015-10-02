TEMPLATE = app

QT += qml quick
CONFIG += c++11

include(../src/libQDeviceWatcher.pri)

win32:!wince*:LIBS += -lUser32

SOURCES += main.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    dplayer.h

INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/
