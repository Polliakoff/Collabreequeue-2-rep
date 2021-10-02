QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
INCLUDEPATH += "$$PWD/eigen-3.4.0"
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    func.cpp \
    brain.cpp \
    main.cpp \
    mainwindow.cpp \
    polygon.cpp \
    straight_line.cpp

HEADERS += \
    func.h \
    brain.h \
    mainwindow.h \
    polygon.h \
    straight_line.h



FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
