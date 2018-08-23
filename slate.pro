TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS += \
    app

!emscripten:SUBDIRS += tests

OTHER_FILES += \
    README.md \
    $$files(doc/*.md)
