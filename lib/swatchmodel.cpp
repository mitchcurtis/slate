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

#include "swatchmodel.h"

#include "project.h"
#include "swatch.h"

SwatchModel::SwatchModel(QObject *parent) :
    QAbstractListModel(parent),
    mProject(nullptr)
{
}

SwatchModel::~SwatchModel()
{
}

Project *SwatchModel::project() const
{
    return mProject;
}

void SwatchModel::setProject(Project *project)
{
    if (project == mProject)
        return;

    if (mProject)
        mProject->swatch()->disconnect(this);

    beginResetModel();

    mProject = project;

    endResetModel();
    emit projectChanged();

    if (mProject) {
        connect(mProject->swatch(), &Swatch::preColourAdded, this, &SwatchModel::onPreColourAdded);
        connect(mProject->swatch(), &Swatch::postColourAdded, this, &SwatchModel::onPostColourAdded);
        connect(mProject->swatch(), &Swatch::colourRenamed, this, &SwatchModel::onColourRenamed);
        connect(mProject->swatch(), &Swatch::preColourRemoved, this, &SwatchModel::onPreColourRemoved);
        connect(mProject->swatch(), &Swatch::postColourRemoved, this, &SwatchModel::onPostColourRemoved);
    }
}

QVariant SwatchModel::data(const QModelIndex &index, int role) const
{
    if (!mProject || !checkIndex(index, CheckIndexOption::IndexIsValid))
        return QVariant();

    const SwatchColour swatchColour = mProject->swatch()->colours().at(index.row());

    if (role == NameRole) {
        return QVariant::fromValue(swatchColour.name());
    } else if (role == ColourRole) {
        return QVariant::fromValue(swatchColour.colour());
    }

    return QVariant();
}

int SwatchModel::rowCount(const QModelIndex &) const
{
    if (!mProject)
        return 0;

    return mProject->swatch()->colours().size();
}

int SwatchModel::columnCount(const QModelIndex &) const
{
    return 1;
}

QHash<int, QByteArray> SwatchModel::roleNames() const
{
    QHash<int, QByteArray> names;
    names.insert(NameRole, "name");
    names.insert(ColourRole, "colour");
    return names;
}

void SwatchModel::onPreColourAdded()
{
    const int swatchColourCount = mProject->swatch()->colours().size();
    beginInsertRows(QModelIndex(), swatchColourCount, swatchColourCount);
}

void SwatchModel::onPostColourAdded()
{
    endInsertRows();
}

void SwatchModel::onColourRenamed(int index)
{
    QVector<int> roles;
    roles.append(NameRole);
    const QModelIndex modelIndex(createIndex(index, 0));
    emit dataChanged(modelIndex, modelIndex, roles);
}

void SwatchModel::onPreColourRemoved(int index)
{
    beginRemoveRows(QModelIndex(), index, index);
}

void SwatchModel::onPostColourRemoved()
{
    endRemoveRows();
}
