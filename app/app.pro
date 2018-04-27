TEMPLATE = app
TARGET = slate

OBJECTS_DIR = .obj
MOC_DIR = .moc

include(app.pri)

SOURCES += main.cpp

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)
