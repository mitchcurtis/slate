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

    cpp.cxxLanguageVersion: "c++17"
    // https://bugreports.qt.io/browse/QBS-1655
    Properties {
        condition: qbs.targetOS.contains("windows")
        cpp.driverFlags: ["/Zc:__cplusplus"]
    }
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

    files: [
        "application.h",
        "application.cpp",
        "main.cpp",
        "config.qrc",
        "fonts/fonts.qrc",
        "images/images.qrc",
        // Make docs available in Creator's locator
        "../README.md",
        "../doc/animation-tutorial.md",
        "../doc/getting-started-layered.md",
        "../doc/getting-started-tileset.md",
        "../doc/overview.md",
        "../doc/user-interface.md",
        "../doc/textured-fill-tool-tutorial.md"
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
        bundle.infoPlist: ({
            "NSSupportsAutomaticGraphicsSwitching": "true"
        })
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

    // Linux desktop + icon stuff.
    Group {
        name: "Desktop file (Linux)"
        condition: qbs.targetOS.contains("linux")
        qbs.install: true
        qbs.installDir: "share/applications"
        files: ["Slate.desktop"]
    }

    Group {
        name: "Icon 16x16 (Linux)"
        condition: qbs.targetOS.contains("linux")
        qbs.install: true
        qbs.installDir: "share/icons/hicolor/16x16/apps"
        // Just reuse the macOS files for simplicity.
        files: [ "images/logo/slate.xcassets/slate-icon-mac.appiconset/slate-icon-16.png" ]
    }

    Group {
        name: "Icon 32x32 (Linux)"
        condition: qbs.targetOS.contains("linux")
        qbs.install: true
        qbs.installDir: "share/icons/hicolor/32x32/apps"
        files: [ "images/logo/slate.xcassets/slate-icon-mac.appiconset/slate-icon-32.png" ]
    }

    Group {
         name: "Icon scalable (Linux)"
         condition: qbs.targetOS.contains("linux")
         qbs.install: true
         qbs.installDir: "share/icons/hicolor/scalable/apps"
         files: [ "images/logo/slate-icon-web.svg" ]
     }
}
