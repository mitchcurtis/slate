import qbs

DynamicLibrary {
    targetName: "lib"

    Depends { name: "cpp" }
    Depends { name: "Qt"; submodules: ["core", "gui", "quick", "widgets"]; versionAtLeast: "5.12" }

    cpp.cxxLanguageVersion: "c++11"
    cpp.visibility: "minimal"
    cpp.defines: [
        "SLATE_LIBRARY",
//        "QT_NO_CAST_FROM_ASCII",
//        "QT_NO_CAST_TO_ASCII",
//        "QT_NO_URL_CAST_FROM_STRING",
//        "_USE_MATH_DEFINES"
    ]

//    Properties {
//        condition: qbs.targetOS.contains("macos")
//        cpp.cxxFlags: ["-Wno-unknown-pragmas"]
//    }

//    bundle.isBundle: false
//    cpp.sonamePrefix: qbs.targetOS.contains("darwin") ? "@rpath" : undefined

    Export {
        Depends { name: "cpp" }
        Depends {
            name: "Qt"
            submodules: ["core", "gui", "quick", "widgets"]
        }

//        cpp.includePaths: "."
        cpp.includePaths: [product.sourceDirectory]
    }

//    Group {
//        qbs.install: true
//        qbs.installDir: {
//            if (qbs.targetOS.contains("windows"))
//                return ""
//            else if (qbs.targetOS.contains("darwin"))
//                return "Tiled.app/Contents/Frameworks"
//            else
//                return "lib"
//        }
//        fileTagsFilter: "dynamiclibrary"
//    }

    files: [
        "addguidecommand.cpp",
        "addguidecommand.h",
        "addlayercommand.cpp",
        "addlayercommand.h",
        "animationplayback.cpp",
        "animationplayback.h",
        "applicationsettings.cpp",
        "applicationsettings.h",
        "applygreedypixelfillcommand.cpp",
        "applygreedypixelfillcommand.h",
        "applypixelerasercommand.cpp",
        "applypixelerasercommand.h",
        "applypixelfillcommand.cpp",
        "applypixelfillcommand.h",
        "applypixellinecommand.cpp",
        "applypixellinecommand.h",
        "applypixelpencommand.cpp",
        "applypixelpencommand.h",
        "applytilecanvaspixelfillcommand.cpp",
        "applytilecanvaspixelfillcommand.h",
        "applytileerasercommand.cpp",
        "applytileerasercommand.h",
        "applytilefillcommand.cpp",
        "applytilefillcommand.h",
        "applytilepencommand.cpp",
        "applytilepencommand.h",
        "autoswatchmodel.cpp",
        "autoswatchmodel.h",
        "canvaspane.cpp",
        "canvaspane.h",
        "canvaspaneitem.cpp",
        "canvaspaneitem.h",
        "changeimagecanvassizecommand.cpp",
        "changeimagecanvassizecommand.h",
        "changeimagesizecommand.cpp",
        "changeimagesizecommand.h",
        "changelayeredimagecanvassizecommand.cpp",
        "changelayeredimagecanvassizecommand.h",
        "changelayeredimagesizecommand.cpp",
        "changelayeredimagesizecommand.h",
        "changelayernamecommand.cpp",
        "changelayernamecommand.h",
        "changelayeropacitycommand.cpp",
        "changelayeropacitycommand.h",
        "changelayerordercommand.cpp",
        "changelayerordercommand.h",
        "changelayervisiblecommand.cpp",
        "changelayervisiblecommand.h",
        "changetilecanvassizecommand.cpp",
        "changetilecanvassizecommand.h",
        "commands.h",
        "crophelper.cpp",
        "crophelper.h",
        "deleteguidecommand.cpp",
        "deleteguidecommand.h",
        "deleteimagecanvasselectioncommand.cpp",
        "deleteimagecanvasselectioncommand.h",
        "deletelayercommand.cpp",
        "deletelayercommand.h",
        "duplicatelayercommand.cpp",
        "duplicatelayercommand.h",
        "filevalidator.cpp",
        "filevalidator.h",
        "fillalgorithms.cpp",
        "fillalgorithms.h",
        "flipimagecanvasselectioncommand.cpp",
        "flipimagecanvasselectioncommand.h",
        "guide.cpp",
        "guide.h",
        "guidesitem.cpp",
        "guidesitem.h",
        "imagecanvas.cpp",
        "imagecanvas.h",
        "imagelayer.cpp",
        "imagelayer.h",
        "imageproject.cpp",
        "imageproject.h",
        "jsonutils.cpp",
        "jsonutils.h",
        "keysequenceeditor.cpp",
        "keysequenceeditor.h",
        "layeredimagecanvas.cpp",
        "layeredimagecanvas.h",
        "layeredimageproject.cpp",
        "layeredimageproject.h",
        "layermodel.cpp",
        "layermodel.h",
        "mergelayerscommand.cpp",
        "mergelayerscommand.h",
        "moveguidecommand.cpp",
        "moveguidecommand.h",
        "moveimagecanvasselectioncommand.cpp",
        "moveimagecanvasselectioncommand.h",
        "movelayeredimagecontentscommand.cpp",
        "movelayeredimagecontentscommand.h",
        "newprojectvalidator.cpp",
        "newprojectvalidator.h",
        "panedrawinghelper.cpp",
        "panedrawinghelper.h",
        "pasteimagecanvascommand.cpp",
        "pasteimagecanvascommand.h",
        "project.cpp",
        "project.h",
        "projectimageprovider.cpp",
        "projectimageprovider.h",
        "projectmanager.cpp",
        "projectmanager.h",
        "rectangularcursor.cpp",
        "rectangularcursor.h",
        "ruler.cpp",
        "ruler.h",
        "saturationlightnesspicker.cpp",
        "saturationlightnesspicker.h",
        "selectioncursorguide.cpp",
        "selectioncursorguide.h",
        "selectionitem.cpp",
        "selectionitem.h",
        "slate-global.h",
        "splitter.cpp",
        "splitter.h",
        "spriteimage.cpp",
        "spriteimage.h",
        "spriteimageprovider.cpp",
        "spriteimageprovider.h",
        "swatch.cpp",
        "swatch.h",
        "swatchcolour.cpp",
        "swatchcolour.h",
        "swatchmodel.cpp",
        "swatchmodel.h",
        "texturedfillparameters.cpp",
        "texturedfillparameters.h",
        "texturedfillpreviewitem.cpp",
        "texturedfillpreviewitem.h",
        "tile.cpp",
        "tile.h",
        "tilecanvas.cpp",
        "tilecanvas.h",
        "tilecanvaspaneitem.cpp",
        "tilecanvaspaneitem.h",
        "tilegrid.cpp",
        "tilegrid.h",
        "tileset.cpp",
        "tileset.h",
        "tilesetproject.cpp",
        "tilesetproject.h",
        "tilesetswatchimage.cpp",
        "tilesetswatchimage.h",
        "utils.cpp",
        "utils.h",
    ]
}
