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

#include "autoswatchmodel.h"
#include "buildinfo.h"
#include "canvaspane.h"
#include "canvaspaneitem.h"
#include "clipboard.h"
#include "filevalidator.h"
#include "imagecanvas.h"
#include "imagelayer.h"
#include "imageproject.h"
#include "keysequenceeditor.h"
#include "layeredimagecanvas.h"
#include "layermodel.h"
#include "layeredimageproject.h"
#include "newprojectvalidator.h"
#include "probabilityswatchmodel.h"
#include "project.h"
#include "projectimageprovider.h"
#include "projectmanager.h"
#include "rectangularcursor.h"
#include "saturationlightnesspicker.h"
#include "serialisablestate.h"
#include "splitter.h"
#include "spriteimage.h"
#include "spriteimageprovider.h"
#include "swatchmodel.h"
#include "texturedfillparameters.h"
#include "texturedfillpreviewitem.h"
#include "tile.h"
#include "tilecanvas.h"
#include "tilecanvaspaneitem.h"
#include "tilegrid.h"
#include "tileset.h"
#include "tilesetproject.h"
#include "tilesetswatchimage.h"

Q_LOGGING_CATEGORY(lcApplication, "app.application")

static QGuiApplication *createApplication(int &argc, char **argv, const QString &applicationName)
{
    // TODO: move the test rules to test code if possible - e.g. testhelper.cpp
    QLoggingCategory::setFilterRules("app.* = false\ntests.* = false\nui.* = false");

    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication::setOrganizationName("Mitch Curtis");
    QApplication::setApplicationName(applicationName);
    QApplication::setOrganizationDomain("mitchcurtis");
    QApplication::setApplicationDisplayName("Slate - Pixel Art Editor");
    QApplication::setApplicationVersion("0.9.0");

    QApplication *app = new QApplication(argc, argv);
    return app;
}

static QObject *buildInfoSingletonProvider(QQmlEngine *, QJSEngine *)
{
    return new BuildInfo;
}

QObject *clipboardSingletonTypeProvider(QQmlEngine *, QJSEngine *)
{
    return new Clipboard;
}

Application::Application(int &argc, char **argv, const QString &applicationName) :
    mApplication(createApplication(argc, argv, applicationName)),
    mSettings(new ApplicationSettings),
    mEngine(new QQmlApplicationEngine)
{
    qCDebug(lcApplication) << "constructing Application...";

    qmlRegisterType<AutoSwatchModel>("App", 1, 0, "AutoSwatchModel");
    qmlRegisterType<FileValidator>("App", 1, 0, "FileValidator");
    qmlRegisterType<ImageCanvas>();
    qmlRegisterType<ImageCanvas>("App", 1, 0, "ImageCanvas");
    qmlRegisterType<CanvasPaneItem>("App", 1, 0, "CanvasPaneItem");
    qmlRegisterType<TileCanvasPaneItem>("App", 1, 0, "TileCanvasPaneItem");
    qmlRegisterType<KeySequenceEditor>("App", 1, 0, "KeySequenceEditor");
    qmlRegisterType<LayeredImageCanvas>("App", 1, 0, "LayeredImageCanvas");
    qmlRegisterType<LayerModel>("App", 1, 0, "LayerModel");
    qmlRegisterType<NewProjectValidator>("App", 1, 0, "NewProjectValidator");
    qmlRegisterType<ProbabilitySwatch>();
    qmlRegisterType<ProbabilitySwatchModel>("App", 1, 0, "ProbabilitySwatchModel");
    qmlRegisterType<ProjectManager>("App", 1, 0, "ProjectManager");
    qmlRegisterType<RectangularCursor>("App", 1, 0, "RectangularCursor");
    qmlRegisterType<SaturationLightnessPicker>("App", 1, 0, "SaturationLightnessPickerTemplate");
    qmlRegisterType<SerialisableState>();
    qmlRegisterType<SpriteImage>("App", 1, 0, "SpriteImage");
    qmlRegisterType<Splitter>();
    qmlRegisterType<Swatch>();
    qmlRegisterType<SwatchModel>("App", 1, 0, "SwatchModel");
    qmlRegisterType<TexturedFillPreviewItem>("App", 1, 0, "TexturedFillPreviewItem");
    qmlRegisterType<TileCanvas>();
    qmlRegisterType<TileCanvas>("App", 1, 0, "TileCanvas");
    qmlRegisterType<TileGrid>("App", 1, 0, "TileGrid");
    qmlRegisterType<TilesetSwatchImage>("App", 1, 0, "TilesetSwatchImage");
    qmlRegisterUncreatableType<AnimationPlayback>("App", 1, 0, "AnimationPlayback", QLatin1String("Cannot create objects of type AnimationPlayback"));
    qmlRegisterUncreatableType<CanvasPane>("App", 1, 0, "CanvasPane", "Can't create instances of CanvasPane");
    qmlRegisterUncreatableType<ClipboardImage>("App", 1, 0, "ClipboardImage",
        QLatin1String("Cannot create objects of type ClipboardImage"));
    qmlRegisterUncreatableType<Project>("App", 1, 0, "Project", QLatin1String("Cannot create objects of type Project"));
    qmlRegisterUncreatableType<LayeredImageProject>("App", 1, 0, "LayeredImageProject",
        QLatin1String("Cannot create objects of type LayeredImageProject"));
    qmlRegisterUncreatableType<TexturedFillVarianceParameter>("App", 1, 0, "TexturedFillParameter",
        QLatin1String("Cannot create objects of type TexturedFillParameter"));
    qmlRegisterUncreatableType<TexturedFillParameters>("App", 1, 0, "TexturedFillParameters",
        QLatin1String("Cannot create objects of type TexturedFillParameters"));
    qmlRegisterSingletonType<BuildInfo>("App", 1, 0, "BuildInfo", buildInfoSingletonProvider);
    qmlRegisterSingletonType<Clipboard>("App", 1, 0, "Clipboard", clipboardSingletonTypeProvider);
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

    // Install fonts. It's especially important to ensure that all fonts we use
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

    // Install a translator for the current language.
    QTranslator translator;
    const QLocale locale(mSettings->language());
    QDir translationsDir = QDir::current();
    translationsDir.cdUp();
    translationsDir.cd(QStringLiteral("Translations"));
    if (translator.load(locale, QStringLiteral("slate_"), QString(), translationsDir.absolutePath())) {
        mApplication->installTranslator(&translator);
    }

#if defined(Q_OS_WIN) || defined(Q_OS_MACOS)
    QQmlFileSelector fileSelector(mEngine.data());
    fileSelector.setExtraSelectors(QStringList() << QLatin1String("nativemenubar"));
#endif

    mProjectManager.setApplicationSettings(mSettings.data());

    mEngine->addImageProvider("sprite", new SpriteImageProvider);
    mEngine->addImageProvider("project", new ProjectImageProvider(&mProjectManager));

    mEngine->rootContext()->setContextProperty("projectManager", &mProjectManager);
    mEngine->rootContext()->setContextProperty("settings", mSettings.data());

    qCDebug(lcApplication) << "Loading main.qml...";
    mEngine->load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    qCDebug(lcApplication) << "... loaded main.qml";
    Q_ASSERT(!mEngine->rootObjects().isEmpty());

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
