/*
    Copyright 2017, Mitch Curtis

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

#include "layermodel.h"

#include "imagelayer.h"
#include "layeredimageproject.h"

LayerModel::LayerModel(QObject *parent) :
    QAbstractListModel(parent),
    mLayeredImageProject(nullptr)
{
}

LayeredImageProject *LayerModel::layeredImageProject() const
{
    return mLayeredImageProject;
}

void LayerModel::setLayeredImageProject(LayeredImageProject *layeredImageProject)
{
    if (layeredImageProject == mLayeredImageProject)
        return;

    if (mLayeredImageProject) {
        mLayeredImageProject->disconnect(this);
    }

    beginResetModel();
    mLayeredImageProject = layeredImageProject;
    endResetModel();
    emit layeredImageProjectChanged();

    if (mLayeredImageProject) {
        connect(mLayeredImageProject, &LayeredImageProject::preLayerAdded, this, &LayerModel::onPreLayerAdded);
        connect(mLayeredImageProject, &LayeredImageProject::postLayerAdded, this, &LayerModel::onPostLayerAdded);
        connect(mLayeredImageProject, &LayeredImageProject::preLayerRemoved, this, &LayerModel::onPreLayerRemoved);
        connect(mLayeredImageProject, &LayeredImageProject::postLayerRemoved, this, &LayerModel::onPostLayerRemoved);
    }
}

QVariant LayerModel::data(const QModelIndex &index, int role) const
{
    if (!mLayeredImageProject || !index.isValid())
        return QVariant();

    ImageLayer *layer = mLayeredImageProject->layerAt(index.row());
    if (!layer)
        return QVariant();

    if (role == LayerRole) {
        return QVariant::fromValue(layer);
    }
    return QVariant();
}

int LayerModel::rowCount(const QModelIndex &) const
{
    if (!mLayeredImageProject)
        return 0;

    return mLayeredImageProject->layerCount();
}

int LayerModel::columnCount(const QModelIndex &) const
{
//    if (!mLayeredImageProject)
//        return 0;

//    return roleNames().size();
    return 1;
}

QHash<int, QByteArray> LayerModel::roleNames() const
{
    QHash<int, QByteArray> names;
    names.insert(LayerRole, "layer");
    return names;
}

void LayerModel::onPreLayerAdded(int index)
{
    beginInsertRows(QModelIndex(), index, index);
}

void LayerModel::onPostLayerAdded(int)
{
    endInsertRows();
}

void LayerModel::onPreLayerRemoved(int index)
{
    beginRemoveRows(QModelIndex(), index, index);
}

void LayerModel::onPostLayerRemoved(int)
{
    endRemoveRows();
}
