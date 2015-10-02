TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = libqdevicewatcher app

libqdevicewatcher.file = src/libQDeviceWatcher.pro
app.file = app/DPlayer.pro
app.depends += libqdevicewatcher
