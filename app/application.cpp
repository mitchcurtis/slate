/*
    Copyright 2020, Mitch Curtis

    This file is part of Slate.

    Slate is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Slate is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Slate. If not, see <http://www.gnu.org/licenses/>.
*/

#include "application.h"

#include <QApplication>
#include <QFontDatabase>
#include <QLoggingCategory>
#include <QQmlFileSelector>
#include <QUndoStack>

#include "canvaspane.h"
#include "canvaspaneitem.h"
#include "imagecanvas.h"
#include "imagelayer.h"
#include "imageproject.h"
#include "layeredimagecanvas.h"
#include "layeredimageproject.h"
#include "project.h"
#include "projectimageprovider.h"
#include "projectmanager.h"
#include "spriteimageprovider.h"
#include "tilecanvas.h"
#include "tilecanvaspaneitem.h"
#include "tileset.h"
#include "tilesetproject.h"

Q_LOGGING_CATEGORY(lcApplication, "app.application")

static QGuiApplication *createApplication(int &argc, char **argv, const QString &applicationName)
{
    // TODO: move the test rules to test code if possible - e.g. testhelper.cpp
    QLoggingCategory::setFilterRules("app.* = false\ntests.* = false\nui.* = false");

    // High-DPI scaling is always enabled in Qt 6.
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QApplication::setOrganizationName("Mitch Curtis");
    QApplication::setApplicationName(applicationName);
    QApplication::setOrganizationDomain("mitchcurtis");
    QApplication::setApplicationDisplayName("Slate - Pixel Art Editor");
    QApplication::setApplicationVersion("0.10.0");

    QApplication *app = new QApplication(argc, argv);
    return app;
}

Application::Application(int &argc, char **argv, const QString &applicationName) :
    mApplication(createApplication(argc, argv, applicationName)),
    mSettings(new ApplicationSettings),
    mEngine(new QQmlApplicationEngine)
{
    qCDebug(lcApplication) << "constructing Application...";

    registerQmlTypes();

    addFonts();

    installTranslators();

#if defined(Q_OS_WIN) || defined(Q_OS_MACOS)
    QQmlFileSelector fileSelector(mEngine.data());
    fileSelector.setExtraSelectors(QStringList() << QLatin1String("nativemenubar"));
#endif

    mProjectManager.setApplicationSettings(mSettings.data());

    mEngine->addImageProvider("sprite", new SpriteImageProvider);
    mEngine->addImageProvider("project", new ProjectImageProvider(&mProjectManager));

    mEngine->rootContext()->setContextProperty("projectManager", &mProjectManager);
    mEngine->rootContext()->setContextProperty("settings", mSettings.data());
    mEngine->rootContext()->setContextProperty("qtVersion", QT_VERSION_STR);

    qCDebug(lcApplication) << "Loading main.qml...";
    mEngine->load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    if (!mEngine->rootObjects().isEmpty())
        qCDebug(lcApplication) << "... loaded main.qml";
    else
        qCDebug(lcApplication) << "... failed to load main.qml";

    qCDebug(lcApplication) << "... constructed Application";
}

Application::~Application()
{
    // Context properties should outlive QML.
    // Usually this wouldn't be a problem with a traditional main(),
    // but since we're trying to encapsulate as much as possible into Application
    // to be able to reuse it in tests, we run into the problem where
    // the project manager (a context property) is destroyed before the engine.
    // Give the engine a little helping hand and ensure that it's
    // destroyed before the project manager, otherwise we get binding errors.
    mEngine.reset();
}

int Application::run()
{
    return mApplication->exec();
}

ApplicationSettings *Application::settings() const
{
    return mSettings.data();
}

QQmlApplicationEngine *Application::qmlEngine() const
{
    return mEngine.data();
}

ProjectManager *Application::projectManager()
{
    return &mProjectManager;
}

void Application::registerQmlTypes()
{
    qRegisterMetaType<ApplicationSettings*>();
    qRegisterMetaType<ImageLayer*>();
    qRegisterMetaType<Project::Type>();

    // For some reason, only when debugging, I get
    // QMetaProperty::read: Unable to handle unregistered datatype 'QUndoStack*' for property 'Project_QML_108::undoStack'
    // if I don't do this.
    qRegisterMetaType<QUndoStack*>();
    qRegisterMetaType<Tile*>();
    qRegisterMetaType<Tileset*>();
    qRegisterMetaType<QVector<QColor>>();
}

void Application::addFonts()
{
    // It's especially important to ensure that all fonts we use
    // are available, otherwise Qt will have to search for "font family aliases",
    // which can take 1 second (observable with qt.qpa.fonts.warning = true).
    const QVector<QString> fontsToLoad = {
        QStringLiteral(":/fonts/FontAwesome.otf"),
        QStringLiteral(":/fonts/Roboto/Roboto-Bold.ttf"),
        QStringLiteral(":/fonts/Roboto/Roboto-Regular.ttf")
    };
    for (const QString &fontPath : fontsToLoad) {
        if (QFontDatabase::addApplicationFont(fontPath) == -1) {
            qWarning() << "Failed to load font:" << fontPath;
        }
    }
}

void Application::installTranslators()
{
    // Install translators for the current language.
    const QLocale locale(mSettings->language());

    QDir slateTranslationsDir = QDir::current();
#if defined(Q_OS_WIN32)
    slateTranslationsDir.cd(QStringLiteral("translations"));
#elif defined(Q_OS_MAC)
    slateTranslationsDir.cdUp();
    slateTranslationsDir.cd(QStringLiteral("Translations"));
#else
    slateTranslationsDir.cd(QStringLiteral("translations"));
#endif
    qCDebug(lcApplication) << "looking for translation for"
        << locale.name() << "locale in" << slateTranslationsDir.absolutePath();

    QTranslator *slateTranslator = new QTranslator(mApplication.data());
    if (slateTranslator->load(locale, QStringLiteral("slate_"), QString(), slateTranslationsDir.absolutePath())) {
        mApplication->installTranslator(slateTranslator);
    } else {
        qWarning() << "Failed to load slate_* translation for locale"
            << locale.name() << "from" << slateTranslationsDir.absolutePath();
        delete slateTranslator;
        slateTranslator = nullptr;
    }

    const QString qtTranslationsDir = QLibraryInfo::path(QLibraryInfo::TranslationsPath);
    QTranslator *qtTranslator = new QTranslator(mApplication.data());
    if (qtTranslator->load(locale, QStringLiteral("qt_"), QString(), qtTranslationsDir)) {
        mApplication->installTranslator(qtTranslator);
    } else {
        qWarning() << "Failed to load qt_* translation for locale"
            << locale.name() << "from" << qtTranslationsDir;
        delete qtTranslator;
        qtTranslator = nullptr;
    }
}
