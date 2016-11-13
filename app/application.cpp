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

#include <QFontDatabase>
#include <QLoggingCategory>

#include "tilecanvas.h"
#include "tilecanvaspane.h"
#include "filevalidator.h"
#include "keysequenceeditor.h"
#include "newprojectvalidator.h"
#include "project.h"
#include "rectangularcursor.h"
#include "splitter.h"
#include "tile.h"
#include "tilegrid.h"
#include "tileset.h"
#include "tilesetswatchimage.h"

Q_LOGGING_CATEGORY(lcApplication, "app.application")

static QGuiApplication *createApplication(int &argc, char **argv, const QString &applicationName)
{
    qputenv("QT_QUICK_CONTROLS_HOVER", "1");
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication *app = new QGuiApplication(argc, argv);
    app->setOrganizationName("Mitch Curtis");
    app->setApplicationName(applicationName);
    app->setOrganizationDomain("mitchcurtis");
    app->setApplicationDisplayName("Slate Tileset Editor");
    app->setApplicationVersion("0.0");
    return app;
}

Application::Application(int &argc, char **argv, const QString &applicationName) :
    mApplication(createApplication(argc, argv, applicationName)),
    mSettings(new Settings),
    mEngine(new QQmlApplicationEngine)
{
    qmlRegisterType<Project>("App", 1, 0, "Project");
    qmlRegisterType<TileCanvas>("App", 1, 0, "TileCanvas");
    qmlRegisterType<TileCanvas>();
    qmlRegisterType<Splitter>();
    qmlRegisterUncreatableType<TileCanvasPane>("App", 1, 0, "CanvasPane", "Can't create instances of CanvasPane");
    qmlRegisterType<FileValidator>("App", 1, 0, "FileValidator");
    qmlRegisterType<NewProjectValidator>("App", 1, 0, "NewProjectValidator");
    qmlRegisterType<RectangularCursor>("App", 1, 0, "RectangularCursor");
    qmlRegisterType<TilesetSwatchImage>("App", 1, 0, "TilesetSwatchImage");
    qmlRegisterType<KeySequenceEditor>("App", 1, 0, "KeySequenceEditor");
    qmlRegisterType<TileGrid>("App", 1, 0, "TileGrid");

    // For some reason, only when debugging, I get
    // QMetaProperty::read: Unable to handle unregistered datatype 'UndoStack*' for property 'Project_QML_108::undoStack'
    // if I don't do this.
    qRegisterMetaType<UndoStack*>();
    qRegisterMetaType<Tile*>();
    qRegisterMetaType<Tileset*>();

    if (QFontDatabase::addApplicationFont(":/fonts/FontAwesome.otf") == -1) {
        qWarning() << "Failed to load FontAwesome font";
    }

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

Settings *Application::settings() const
{
    return mSettings.data();
}

QQmlApplicationEngine *Application::qmlEngine() const
{
    return mEngine.data();
}
