/*
    Copyright 2016, Mitch Curtis

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

#ifndef NON_NATIVE_MENUS
#include <QApplication>
#endif
#include <QFontDatabase>
#include <QLoggingCategory>

#include "canvaspane.h"
#include "filevalidator.h"
#include "imagecanvas.h"
#include "imagelayer.h"
#include "imageproject.h"
#include "keysequenceeditor.h"
#include "layeredimagecanvas.h"
#include "layermodel.h"
#include "layeredimageproject.h"
#include "newprojectvalidator.h"
#include "project.h"
#include "projectmanager.h"
#include "rectangularcursor.h"
#include "simpleloader.h"
#include "splitter.h"
#include "spriteimage.h"
#include "spriteimageprovider.h"
#include "tile.h"
#include "tilecanvas.h"
#include "tilegrid.h"
#include "tileset.h"
#include "tilesetproject.h"
#include "tilesetswatchimage.h"

Q_LOGGING_CATEGORY(lcApplication, "app.application")

#ifdef NON_NATIVE_MENUS
typedef QGuiApplication QtApplicationType;
#else
typedef QApplication QtApplicationType;
#endif

static QGuiApplication *createApplication(int &argc, char **argv, const QString &applicationName)
{
    QLoggingCategory::setFilterRules("app.* = false");
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QtApplicationType *app = new QtApplicationType(argc, argv);
    app->setOrganizationName("Mitch Curtis");
    app->setApplicationName(applicationName);
    app->setOrganizationDomain("mitchcurtis");
    app->setApplicationDisplayName("Slate - Pixel Art Editor");
    app->setApplicationVersion("0.0");
    return app;
}

Application::Application(int &argc, char **argv, const QString &applicationName) :
    mApplication(createApplication(argc, argv, applicationName)),
    mSettings(new ApplicationSettings),
    mEngine(new QQmlApplicationEngine)
{
    qmlRegisterType<FileValidator>("App", 1, 0, "FileValidator");
    qmlRegisterType<ImageCanvas>();
    qmlRegisterType<ImageCanvas>("App", 1, 0, "ImageCanvas");
    qmlRegisterType<KeySequenceEditor>("App", 1, 0, "KeySequenceEditor");
    qmlRegisterType<LayeredImageCanvas>("App", 1, 0, "LayeredImageCanvas");
    qmlRegisterType<LayerModel>("App", 1, 0, "LayerModel");
    qmlRegisterType<NewProjectValidator>("App", 1, 0, "NewProjectValidator");
    qmlRegisterType<ProjectManager>("App", 1, 0, "ProjectManager");
    qmlRegisterType<RectangularCursor>("App", 1, 0, "RectangularCursor");
    qmlRegisterType<SimpleLoader>("App", 1, 0, "SimpleLoader");
    qmlRegisterType<SpriteImage>("App", 1, 0, "SpriteImage");
    qmlRegisterType<Splitter>();
    qmlRegisterType<TileCanvas>();
    qmlRegisterType<TileCanvas>("App", 1, 0, "TileCanvas");
    qmlRegisterType<TileGrid>("App", 1, 0, "TileGrid");
    qmlRegisterType<TilesetSwatchImage>("App", 1, 0, "TilesetSwatchImage");
    qmlRegisterUncreatableType<AnimationPlayback>("App", 1, 0, "AnimationPlayback", QLatin1String("Cannot create objects of type AnimationPlayback"));
    qmlRegisterUncreatableType<CanvasPane>("App", 1, 0, "CanvasPane", "Can't create instances of CanvasPane");
    qmlRegisterUncreatableType<Project>("App", 1, 0, "Project", QLatin1String("Cannot create objects of type Project"));
    qmlRegisterUncreatableType<LayeredImageProject>("App", 1, 0, "LayeredImageProject",
        QLatin1String("Cannot create objects of type LayeredImageProject"));

    qRegisterMetaType<ApplicationSettings*>();
    qRegisterMetaType<ImageLayer*>();
    qRegisterMetaType<Project::Type>();
    qRegisterMetaType<Tile*>();
    qRegisterMetaType<Tileset*>();
    qRegisterMetaType<UndoStack*>();

    if (QFontDatabase::addApplicationFont(":/fonts/FontAwesome.otf") == -1) {
        qWarning() << "Failed to load FontAwesome font";
    }

    mEngine->addImageProvider("sprite", new SpriteImageProvider);

    mEngine->rootContext()->setContextProperty("settings", mSettings.data());
    qCDebug(lcApplication) << "Loading main.qml...";
    mEngine->load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    qCDebug(lcApplication) << "... loaded main.qml";
    Q_ASSERT(!mEngine->rootObjects().isEmpty());
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
