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

#include "autoswatchmodel.h"

#include <QLoggingCategory>

#include "imagecanvas.h"
#include "project.h"

Q_LOGGING_CATEGORY(lcAutoSwatchModel, "app.autoSwatchModel")

AutoSwatchModel::AutoSwatchModel(QObject *parent) :
    QAbstractListModel(parent),
    mCanvas(nullptr)
{
}

ImageCanvas *AutoSwatchModel::canvas() const
{
    return mCanvas;
}

void AutoSwatchModel::setCanvas(ImageCanvas *canvas)
{
    if (canvas == mCanvas)
        return;

    if (mCanvas) {
        mCanvas->disconnect(this);
    }

    beginResetModel();

    mCanvas = canvas;

    endResetModel();
    emit canvasChanged();

    if (mCanvas) {
        connect(mCanvas, &ImageCanvas::projectChanged, this, &AutoSwatchModel::onProjectChanged);
    }
}

QVariant AutoSwatchModel::data(const QModelIndex &index, int role) const
{
    if (!mCanvas || !mCanvas->project() || !checkIndex(index, CheckIndexOption::IndexIsValid))
        return QVariant();

    if (role == ColourRole) {
        const auto colour = mColours.at(index.row());
        return QVariant::fromValue(colour);
    }

    return QVariant();
}

int AutoSwatchModel::rowCount(const QModelIndex &) const
{
    if (!mCanvas || !mCanvas->project())
        return 0;

    return mColours.size();
}

int AutoSwatchModel::columnCount(const QModelIndex &) const
{
    return 1;
}

QHash<int, QByteArray> AutoSwatchModel::roleNames() const
{
    QHash<int, QByteArray> names;
    names.insert(ColourRole, "colour");
    return names;
}

void AutoSwatchModel::onProjectChanged()
{
    if (mCanvas->project()) {
        connect(mCanvas->project()->undoStack(), &QUndoStack::indexChanged,
            this, &AutoSwatchModel::onUndoStackIndexChanged);

        // Force population.
        onUndoStackIndexChanged();
    }
}

void AutoSwatchModel::onUndoStackIndexChanged()
{
    qCDebug(lcAutoSwatchModel) << "populating model...";

    beginResetModel();

    mColours.clear();

    // TODO: cache exported image as a member and only process the changed area?
    const QImage image = mCanvas->project()->exportedImage();
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            const QColor colour = image.pixelColor(x, y);
            if (!mColours.contains(colour))
                mColours.append(colour);
        }
    }

    endResetModel();

    qCDebug(lcAutoSwatchModel) << "... finished populating model";
}
