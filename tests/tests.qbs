import qbs

Project {
    name: "tests"
    minimumQbsVersion: "1.7.1"

    references: [
        "auto/test-app.qbs",
        "manual/screenshots/screenshots.qbs"
    ]
}

