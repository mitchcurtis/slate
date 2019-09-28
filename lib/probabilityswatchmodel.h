/*
    Copyright 2019, Mitch Curtis

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

#ifndef PROBABILITYSWATCHMODEL_H
#define PROBABILITYSWATCHMODEL_H

#include "slate-global.h"
#include "swatchmodel.h"

class SLATE_EXPORT ProbabilitySwatchModel : public SwatchModel
{
    Q_OBJECT

public:
    enum {
        NameRole = Qt::UserRole,
        ColourRole,
        ColourHexNameRole,
        ProbabilityRole
    };

    explicit ProbabilitySwatchModel(QObject *parent = nullptr);
    ~ProbabilitySwatchModel() override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

protected:
    void connectToSwatch() override;

private slots:
    void onProbabilityChanged(int index);
};

#endif // PROBABILITYSWATCHMODEL_H
