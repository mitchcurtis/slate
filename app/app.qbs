import qbs
import "qml/qml.qbs" as AppQmlFiles

QtGuiApplication {
    name: "app"
    targetName: "Slate"
    consoleApplication: false

    Depends { name: "Qt.core" }
    Depends { name: "Qt.quick" }
    Depends { name: "Qt.widgets" }
    Depends { name: "lib" }
    // Provides support for Apple Interface Builder - aka asset catalogs.
    Depends { name: "ib"; condition: qbs.targetOS.contains("macos") }

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

    files: [
        "application.h",
        "application.cpp",
        "main.cpp",
        "config.qrc",
        "fonts/fonts.qrc",
        "images/images.qrc"
    ]

    AppQmlFiles {}

    Group {
        name: "Install (non-macOS)"
        condition: !qbs.targetOS.contains("macos")
        qbs.install: true
        qbs.installSourceBase: product.buildDirectory
        fileTagsFilter: product.type
    }

    // This is necessary to install the app bundle (OS X)
    Group {
        name: "bundle.content install"
        fileTagsFilter: ["bundle.content"]
        qbs.install: true
        qbs.installDir: "."
        qbs.installSourceBase: product.buildDirectory
    }

    // macOS icon stuff.
    Properties {
        condition: qbs.targetOS.contains("macos")
        ib.appIconName: "slate-icon-mac"
//        bundle.infoPlist: {
//            CFBundleIconFile: "images/logo/slate.xcassets/slate-icon-mac.appiconset"
//        }
    }

    Group {
        name: "Icons (macOS)"
        condition: qbs.targetOS.contains("macos")
        files: ["images/logo/slate.xcassets"]
    }

    // Windows icon stuff.
    Group {
        name: "RC file (Windows)"
        files: ["slate.rc"]
    }
}

