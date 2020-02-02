import qbs

Product {
    name: "translations"
    type: "qm"
    files: "*.ts"

    Depends { name: "Qt.core" }

    Group {
        fileTagsFilter: product.type
        qbs.install: true
        qbs.installDir: {
            if (qbs.targetOS.contains("macos"))
                return "Slate.app/Contents/Translations"
            else
                // linuxdeployqt puts Qt's translation files into translations/,
                // so we just do it that way here too.
                return "translations"
        }
    }
}
