import qbs
import qbs.Environment

Project {
    name: "tests"
    minimumQbsVersion: "1.7.1"

    references: {
        var files = [
            "auto/test-app.qbs",
            "manual/screenshots/screenshots.qbs"
        ]

        if (Environment.getEnv("USE_BENCHMARK") === "1")
            files.push("manual/memory-usage/memory-usage.qbs")

        return files
    }
}

