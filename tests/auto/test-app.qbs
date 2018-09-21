import qbs

Project {
    name: "tests"
    minimumQbsVersion: "1.7.1"

    CppApplication {
        Depends { name: "Qt.core" }
        Depends { name: "Qt.quick" }
        Depends { name: "Qt.test" }
        Depends { name: "Qt.widgets" }
        Depends { name: "lib" }

        // Additional import path used to resolve QML modules in Qt Creator's code model
        property pathList qmlImportPaths: []

        cpp.useRPaths: qbs.targetOS.contains("darwin")
        cpp.rpaths: ["@loader_path/../Frameworks"]

        cpp.cxxLanguageVersion: "c++11"

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
            "../shared"
        ]

        files: [
            "../../app/application.h",
            "../../app/application.cpp",
            "../../app/fonts/fonts.qrc",
            "../../app/images/images.qrc",
            "../../app/qml/qml.qrc",
            "resources.qrc",
            "tst_app.cpp",
            "../shared/testhelper.h",
            "../shared/testhelper.cpp",
            "../shared/testutils.h"
        ]

        Group {     // Properties for the produced executable
            fileTagsFilter: "application"
            qbs.install: true
        }
    }
}

