import qbs

Project {
    name: "slate"
    minimumQbsVersion: "1.7.1"

    references: [
        "app/app.qbs",
        "dist/dist.qbs",
        "lib/lib.qbs",
        "tests/tests.qbs",
        "translations/translations.qbs"
    ]
}

