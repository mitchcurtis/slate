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

#include "swatchmodel.h"

#include <QLoggingCategory>

#include "swatch.h"

Q_LOGGING_CATEGORY(lcSwatchModel, "app.swatchModel")

SwatchModel::SwatchModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

SwatchModel::~SwatchModel()
{
}

Swatch *SwatchModel::swatch() const
{
    return mSwatch;
}

void SwatchModel::setSwatch(Swatch *swatch)
{
    qCDebug(lcSwatchModel) << "swatch" << swatch << "set on" << this;
    if (swatch == mSwatch)
        return;

    if (mSwatch)
        mSwatch->disconnect(this);

    beginResetModel();

    mSwatch = swatch;

    endResetModel();
    emit swatchChanged();

    if (mSwatch)
        connectToSwatch();
}

QVariant SwatchModel::data(const QModelIndex &index, int role) const
{
    if (!mSwatch || !checkIndex(index, CheckIndexOption::IndexIsValid))
        return QVariant();

    const SwatchColour swatchColour = mSwatch->colours().at(index.row());

    if (role == NameRole) {
        return QVariant::fromValue(swatchColour.name());
    } else if (role == ColourRole) {
        return QVariant::fromValue(swatchColour.colour());
    } else if (role == ColourHexNameRole) {
        return QVariant::fromValue(swatchColour.colour().name(QColor::HexArgb));
    }

    return QVariant();
}

int SwatchModel::rowCount(const QModelIndex &) const
{
    if (!mSwatch)
        return 0;

    return mSwatch->colours().size();
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
    names.insert(ColourHexNameRole, "colourHexName");
    return names;
}

void SwatchModel::connectToSwatch()
{
    connect(mSwatch, &Swatch::preColourAdded, this, &SwatchModel::onPreColourAdded);
    connect(mSwatch, &Swatch::postColourAdded, this, &SwatchModel::onPostColourAdded);
    connect(mSwatch, &Swatch::preColoursAdded, this, &SwatchModel::onPreColoursAdded);
    connect(mSwatch, &Swatch::postColoursAdded, this, &SwatchModel::onPostColoursAdded);
    connect(mSwatch, &Swatch::colourRenamed, this, &SwatchModel::onColourRenamed);
    connect(mSwatch, &Swatch::preColourRemoved, this, &SwatchModel::onPreColourRemoved);
    connect(mSwatch, &Swatch::postColourRemoved, this, &SwatchModel::onPostColourRemoved);
    connect(mSwatch, &Swatch::preImported, this, &SwatchModel::onPreSwatchImported);
    connect(mSwatch, &Swatch::postImported, this, &SwatchModel::onPostSwatchImported);
}

void SwatchModel::onPreColourAdded()
{
    const int swatchColourCount = mSwatch->colours().size();
    beginInsertRows(QModelIndex(), swatchColourCount, swatchColourCount);
}

void SwatchModel::onPostColourAdded()
{
    endInsertRows();
}

void SwatchModel::onPreColoursAdded()
{
    qCDebug(lcSwatchModel) << "swatch colours about to change (" << this << "); about to call beginResetModel()...";
    beginResetModel();
    qCDebug(lcSwatchModel) << "... called beginResetModel()";
}

void SwatchModel::onPostColoursAdded()
{
    qCDebug(lcSwatchModel) << "swatch colours changed (" << this << "); about to call endResetModel()...";
    endResetModel();
    qCDebug(lcSwatchModel) << "... called endResetModel()";
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

void SwatchModel::onPreSwatchImported()
{
    beginResetModel();
}

void SwatchModel::onPostSwatchImported()
{
    endResetModel();
}
