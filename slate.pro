TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS += \
    3rdparty \
    app
    
!emscripten:SUBDIRS += tests

OTHER_FILES += README.md
