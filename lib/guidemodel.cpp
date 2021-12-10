/*
    Copyright 2021, Mitch Curtis

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

#include "guidemodel.h"

#include <QLoggingCategory>

#include "imagecanvas.h"
#include "project.h"

Q_LOGGING_CATEGORY(lcGuideModel, "app.guideModel")

GuideModel::GuideModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

GuideModel::~GuideModel()
{
}

Project *GuideModel::project() const
{
    return mProject;
}

void GuideModel::setProject(Project *project)
{
    qCDebug(lcGuideModel) << "project changed to" << project;

    if (project == mProject)
        return;

    if (mProject)
        mProject->disconnect(this);

    beginResetModel();

    mProject = project;

    endResetModel();
    emit projectChanged();

    if (mProject) {
        connect(mProject, &Project::preGuidesAdded, this, &GuideModel::onPreGuidesAdded);
        connect(mProject, &Project::postGuidesAdded, this, &GuideModel::onPostGuidesAdded);
        connect(mProject, &Project::guideMoved, this, &GuideModel::onGuideMoved);
        connect(mProject, &Project::preGuidesRemoved, this, &GuideModel::onPreGuidesRemoved);
        connect(mProject, &Project::postGuidesRemoved, this, &GuideModel::onPostGuidesRemoved);
    }
}

ImageCanvas *GuideModel::canvas() const
{
    return mCanvas;
}

void GuideModel::setCanvas(ImageCanvas *canvas)
{
    if (canvas == mCanvas)
        return;

    if (mCanvas)
        disconnect(mCanvas, &ImageCanvas::existingGuideDragged, this, &GuideModel::onGuideMoved);

    mCanvas = canvas;

    if (mCanvas)
        connect(mCanvas, &ImageCanvas::existingGuideDragged, this, &GuideModel::onGuideMoved);

    emit canvasChanged();
}

QVariant GuideModel::data(const QModelIndex &index, int role) const
{
    if (!mProject || !checkIndex(index, CheckIndexOption::IndexIsValid))
        return QVariant();

    const Guide guide = mProject->guides().at(index.row());

    switch (role) {
    case OrientationRole:
        return QVariant::fromValue(guide.orientation());
    case PositionRole: {
        // If this is an existing guide that is currently being dragged, draw it in its dragged position.
        // It's OK if canvas is null; the user can't drag while things are starting up.
        const bool draggingExistingGuide = mCanvas && mCanvas->pressedGuideIndex() != -1 && mCanvas->pressedGuideIndex() == index.row();
        const bool vertical = guide.orientation() == Qt::Vertical;
        const int guidePosition = draggingExistingGuide
            ? (vertical ? mCanvas->cursorSceneX() : mCanvas->cursorSceneY()) : guide.position();
        return QVariant::fromValue(guidePosition);
    } default:
        break;
    }

    return QVariant();
}

int GuideModel::rowCount(const QModelIndex &) const
{
    if (!mProject)
        return 0;

    return mProject->guides().size();
}

int GuideModel::columnCount(const QModelIndex &) const
{
    return 1;
}

QHash<int, QByteArray> GuideModel::roleNames() const
{
    QHash<int, QByteArray> names;
    names.insert(OrientationRole, "orientation");
    names.insert(PositionRole, "position");
    return names;
}

void GuideModel::onProjectChanged()
{
}

void GuideModel::onPreGuidesAdded(int index, int count)
{
    beginInsertRows(QModelIndex(), index, index + count - 1);
}

void GuideModel::onPostGuidesAdded()
{
    endInsertRows();
}

void GuideModel::onPreGuidesRemoved()
{
    qCDebug(lcGuideModel) << "guides about to be removed (" << this << "); about to call beginResetModel()...";
    beginResetModel();
    qCDebug(lcGuideModel) << "... called beginResetModel()";
}

void GuideModel::onPostGuidesRemoved()
{
    qCDebug(lcGuideModel) << "guides removed (" << this << "); about to call endResetModel()...";
    endResetModel();
    qCDebug(lcGuideModel) << "... called endResetModel()";
}

void GuideModel::onGuideMoved(int index)
{
    QVector<int> roles;
    roles.append(PositionRole);
    const QModelIndex modelIndex(createIndex(index, 0));
    emit dataChanged(modelIndex, modelIndex, roles);
}
