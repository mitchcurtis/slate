import qbs
import "../../app/qml/qml.qbs" as AppQmlFiles

QtGuiApplication {
    name: "test-app"

    Depends { name: "Qt.core" }
    Depends { name: "Qt.qmltest" }
    Depends { name: "Qt.quick" }
    Depends { name: "Qt.test" }
    Depends { name: "Qt.widgets" }
    Depends { name: "lib" }
    // Workaround for static builds until https://bugreports.qt.io/browse/QBS-1409 is fixed
    Depends {
        name: "Qt"
        condition: Qt.core.staticBuild && qbs.targetOS.contains("linux")
        submodules: ["qxcb-egl-integration"]
    }

    // Additional import path used to resolve QML modules in Qt Creator's code model
    property pathList qmlImportPaths: []

    readonly property bool darwin: qbs.targetOS.contains("darwin")
    readonly property bool unix: qbs.targetOS.contains("unix")

    cpp.useRPaths: darwin || (unix && !Qt.core.staticBuild)
    // Ensure that e.g. libslate is found.
    cpp.rpaths: darwin ? ["@loader_path/../Frameworks"] : ["$ORIGIN"]

    cpp.cxxLanguageVersion: "c++11"
    // https://bugreports.qt.io/browse/QBS-1434
    cpp.minimumMacosVersion: "10.13"

    cpp.defines: [
        // The following define makes your compiler emit warnings if you use
        // any feature of Qt which as been marked deprecated (the exact warnings
        // depend on your compiler). Please consult the documentation of the
        // deprecated API in order to know how to port your code away from it.
        "QT_DEPRECATED_WARNINGS",

        // You can also make your code fail to compile if you use deprecated APIs.
        // In order to do so, uncomment the following line.
        // You can also select to disable deprecated APIs only up to a certain version of Qt.
        //"QT_DISABLE_DEPRECATED_BEFORE=0x060000" // disables all the APIs deprecated before Qt 6.0.0
    ]

    cpp.includePaths: [
        "../../app",
        "../shared",
        "../../lib/3rdparty"
    ]

    files: [
        "../../app/application.h",
        "../../app/application.cpp",
        "../../app/fonts/fonts.qrc",
        "../../app/images/images.qrc",
        "../../lib/3rdparty/bitmap/bmp.h",
        "../../lib/3rdparty/bitmap/bmp.c",
        "../../lib/3rdparty/bitmap/misc/gif.h",
        "../../lib/3rdparty/bitmap/misc/gif.c",
        "resources.qrc",
        "tst_app.cpp",
        "../shared/testhelper.h",
        "../shared/testhelper.cpp",
        "../shared/testutils.h",
    ]

    AppQmlFiles {}

    // These two groups are a workaround for QTBUG-85748,
    // and can be removed (along with the files) when we build with Qt 6.
    Group {
        name: "teststyle-conf"
        prefix: path + "/resources/"
        fileTags: [ "qt.core.resource_data" ]

        Qt.core.resourcePrefix: "/"

        files: [
            "qtquickcontrols2.conf"
        ]
    }

    // We can't get access to the engine before it loads the QML,
    // and resource paths aren't allowed in QML2_IMPORT_PATH,
    // so install the style to the directory that contains the test executable.
    Group {
        name: "teststyle-qml"
        prefix: path + "/resources/"

        qbs.install: true
        qbs.installDir: "."
        qbs.installSourceBase: path + "/resources/"

        files: [
            "TestStyle/Dialog.qml"
        ]
    }

    Group {     // Properties for the produced executable
        fileTagsFilter: "application"
        qbs.install: true
    }
}

