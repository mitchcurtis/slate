/*
    Copyright 2023, Mitch Curtis

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

#ifndef APPLICATION_H
#define APPLICATION_H

#include <QGuiApplication>
#include <QScopedPointer>
#include <QtQml>

#include "applicationsettings.h"
#include "projectmanager.h"

class Application
{
public:
    Application(int &argc, char **argv, const QString &applicationName);
    ~Application();

    int run();

    ApplicationSettings *settings() const;
    QQmlApplicationEngine *qmlEngine() const;
    ProjectManager *projectManager();

private:
    void registerQmlTypes();
    void addFonts();
    void installTranslators();

    QScopedPointer<QGuiApplication> mApplication;
    QScopedPointer<ApplicationSettings> mSettings;
    QScopedPointer<QQmlApplicationEngine> mEngine;
    ProjectManager mProjectManager;
};

#endif // APPLICATION_H
