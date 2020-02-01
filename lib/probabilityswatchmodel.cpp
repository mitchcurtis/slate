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

#include "probabilityswatchmodel.h"

#include <QLoggingCategory>

#include "probabilityswatch.h"

Q_LOGGING_CATEGORY(lcProbabilitySwatchModel, "app.probabilitySwatchModel")

ProbabilitySwatchModel::ProbabilitySwatchModel(QObject *parent) :
    SwatchModel(parent)
{
}

ProbabilitySwatchModel::~ProbabilitySwatchModel()
{
}

void ProbabilitySwatchModel::connectToSwatch()
{
    auto probabilitySwatch = qobject_cast<ProbabilitySwatch*>(mSwatch);
    if (!probabilitySwatch) {
        qWarning() << "Failed to convert swatch to ProbabilitySwatch!";
        return;
    }

    SwatchModel::connectToSwatch();

    connect(probabilitySwatch, &ProbabilitySwatch::probabilityChanged,
        this, &ProbabilitySwatchModel::onProbabilityChanged);
}

QVariant ProbabilitySwatchModel::data(const QModelIndex &index, int role) const
{
    if (!mSwatch || !checkIndex(index, CheckIndexOption::IndexIsValid))
        return QVariant();

    auto probabilitySwatch = qobject_cast<ProbabilitySwatch*>(mSwatch);
    const SwatchColour swatchColour = probabilitySwatch->colours().at(index.row());

    if (role == NameRole) {
        return QVariant::fromValue(swatchColour.name());
    } else if (role == ColourRole) {
        return QVariant::fromValue(swatchColour.colour());
    } else if (role == ColourHexNameRole) {
        return QVariant::fromValue(swatchColour.colour().name(QColor::HexArgb));
    } else if (role == ProbabilityRole) {
        return QVariant::fromValue(probabilitySwatch->probabilities().at(index.row()));
    }

    return QVariant();
}

QHash<int, QByteArray> ProbabilitySwatchModel::roleNames() const
{
    QHash<int, QByteArray> names;
    names.insert(NameRole, "name");
    names.insert(ColourRole, "colour");
    names.insert(ColourHexNameRole, "colourHexName");
    names.insert(ProbabilityRole, "probability");
    return names;
}

void ProbabilitySwatchModel::onProbabilityChanged(int index)
{
    QVector<int> roles;
    roles.append(ProbabilityRole);
    const QModelIndex modelIndex(createIndex(index, 0));
    emit dataChanged(modelIndex, modelIndex, roles);
}

