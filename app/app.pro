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

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

DISTFILES += \
    android/android-libtemplates.so-deployment-settings.json \
    android/android-libgradle.so-deployment-settings.json \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/libtemplates.so \
    android/libgradle.so \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat \
    android/Makefile \
    android/android-libtemplates.so-deployment-settings.json \
    android/AndroidManifest.xml \
    android/libtemplates.so \
    android/res/values/libs.xml \
    android/build.gradle \
    android/Makefile
