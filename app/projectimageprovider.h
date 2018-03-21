/*
    Copyright 2018, Mitch Curtis

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

#ifndef PROJECTIMAGEPROVIDER_H
#define PROJECTIMAGEPROVIDER_H

#include <QImage>
#include <QString>
#include <QQuickImageProvider>

class ProjectManager;

class ProjectImageProvider : public QQuickImageProvider
{
public:
    ProjectImageProvider(ProjectManager *projectManager);
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;

private:
    ProjectManager *mProjectManager;
};

#endif // PROJECTIMAGEPROVIDER_H
