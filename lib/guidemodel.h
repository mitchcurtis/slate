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

#ifndef GUIDEMODEL_H
#define GUIDEMODEL_H

#include <QAbstractListModel>
#include <QColor>
#include <QImage>
#include <QQmlEngine>
#include <QVector>

#include "slate-global.h"

class ImageCanvas;
class Project;
class Swatch;

class SLATE_EXPORT GuideModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(Project *project READ project WRITE setProject NOTIFY projectChanged)
    Q_PROPERTY(ImageCanvas *canvas READ canvas WRITE setCanvas NOTIFY canvasChanged)
    QML_ELEMENT
    Q_MOC_INCLUDE("imagecanvas.h")
    Q_MOC_INCLUDE("project.h")

public:
    enum {
        OrientationRole = Qt::UserRole,
        PositionRole
    };

    explicit GuideModel(QObject *parent = nullptr);
    ~GuideModel() override;

    Project *project() const;
    void setProject(Project *project);

    ImageCanvas *canvas() const;
    void setCanvas(ImageCanvas *canvas);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

signals:
    void projectChanged();
    void canvasChanged();

private slots:
    void onProjectChanged();
    void onPreGuidesAdded(int index, int count);
    void onPostGuidesAdded();
    void onGuideMoved(int index);
    void onPreGuidesRemoved();
    void onPostGuidesRemoved();

protected:
    Project *mProject = nullptr;
    ImageCanvas *mCanvas = nullptr;
};

#endif // GUIDEMODEL_H
