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
    $$PWD/changetilecanvassizecommand.h \
    $$PWD/applypixelfillcommand.h \
    $$PWD/applytilefillcommand.h \
    $$PWD/floodfill.h \
    $$PWD/keysequenceeditor.h \
    $$PWD/tilecanvas.h \
    $$PWD/utils.h \
    $$PWD/tilegrid.h \
    $$PWD/splitter.h \
    $$PWD/newprojectvalidator.h \
    $$PWD/tilesetproject.h \
    $$PWD/imagecanvas.h \
    $$PWD/canvaspane.h \
    $$PWD/commands.h \
    $$PWD/imageproject.h \
    $$PWD/changeimagecanvassizecommand.h \
    $$PWD/projectmanager.h \
    $$PWD/simpleloader.h \
    $$PWD/moveimagecanvasselectioncommand.h

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
    $$PWD/changetilecanvassizecommand.cpp \
    $$PWD/applypixelfillcommand.cpp \
    $$PWD/applytilefillcommand.cpp \
    $$PWD/floodfill.cpp \
    $$PWD/keysequenceeditor.cpp \
    $$PWD/tilecanvas.cpp \
    $$PWD/utils.cpp \
    $$PWD/tilegrid.cpp \
    $$PWD/splitter.cpp \
    $$PWD/newprojectvalidator.cpp \
    $$PWD/tilesetproject.cpp \
    $$PWD/imagecanvas.cpp \
    $$PWD/canvaspane.cpp \
    $$PWD/imageproject.cpp \
    $$PWD/changeimagecanvassizecommand.cpp \
    $$PWD/projectmanager.cpp \
    $$PWD/simpleloader.cpp \
    $$PWD/moveimagecanvasselectioncommand.cpp

RESOURCES += \
    $$PWD/fonts/fonts.qrc \
    $$PWD/images/images.qrc \
    $$PWD/qml/qml.qrc

# Allow tests to use the default style.
!defined(USE_DEFAULT_STYLE, var) {
    RESOURCES += $$PWD/config.qrc
}
