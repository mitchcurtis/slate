TARGET = tst_app

QT += testlib

USE_DEFAULT_STYLE = 1

include(../app/app.pri)

SOURCES += tst_app.cpp \
    testhelper.cpp

RESOURCES += \
    resources.qrc

HEADERS += \
    testhelper.h
