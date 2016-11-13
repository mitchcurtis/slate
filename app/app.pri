QT += widgets qml quick undo
CONFIG += c++11

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/settings.h \
    $$PWD/application.h \
    $$PWD/project.h \
    $$PWD/applypixelpencommand.h \
    $$PWD/tile.h \
    $$PWD/applytilepencommand.h \
    $$PWD/filevalidator.h \
    $$PWD/tileset.h \
    $$PWD/rectangularcursor.h \
    $$PWD/tilesetswatchimage.h \
    $$PWD/applypixelerasercommand.h \
    $$PWD/applytileerasercommand.h \
    $$PWD/changecanvassizecommand.h \
    $$PWD/applypixelfillcommand.h \
    $$PWD/applytilefillcommand.h \
    $$PWD/floodfill.h \
    $$PWD/keysequenceeditor.h \
    $$PWD/tilecanvas.h \
    $$PWD/tilecanvaspane.h \
    $$PWD/utils.h \
    $$PWD/tilegrid.h \
    $$PWD/splitter.h \
    $$PWD/newprojectvalidator.h

SOURCES += \
    $$PWD/settings.cpp \
    $$PWD/application.cpp \
    $$PWD/project.cpp \
    $$PWD/applypixelpencommand.cpp \
    $$PWD/tile.cpp \
    $$PWD/applytilepencommand.cpp \
    $$PWD/filevalidator.cpp \
    $$PWD/tileset.cpp \
    $$PWD/rectangularcursor.cpp \
    $$PWD/tilesetswatchimage.cpp \
    $$PWD/applypixelerasercommand.cpp \
    $$PWD/applytileerasercommand.cpp \
    $$PWD/changecanvassizecommand.cpp \
    $$PWD/applypixelfillcommand.cpp \
    $$PWD/applytilefillcommand.cpp \
    $$PWD/floodfill.cpp \
    $$PWD/keysequenceeditor.cpp \
    $$PWD/tilecanvas.cpp \
    $$PWD/tilecanvaspane.cpp \
    $$PWD/utils.cpp \
    $$PWD/tilegrid.cpp \
    $$PWD/splitter.cpp \
    $$PWD/newprojectvalidator.cpp

RESOURCES += \
    $$PWD/fonts/fonts.qrc \
    $$PWD/qml/qml.qrc

# Allow tests to use the default style.
!defined(USE_DEFAULT_STYLE, var) {
    RESOURCES += $$PWD/config.qrc
}
