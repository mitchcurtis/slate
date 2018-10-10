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

#ifndef SWATCHMODEL_H
#define SWATCHMODEL_H

#include <QAbstractListModel>
#include <QColor>
#include <QImage>
#include <QVector>

#include "slate-global.h"

class ImageLayer;
class Project;

class SLATE_EXPORT SwatchModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(Project *project READ project WRITE setProject NOTIFY projectChanged)

public:
    enum {
        NameRole = Qt::UserRole,
        ColourRole,
    };

    explicit SwatchModel(QObject *parent = nullptr);
    ~SwatchModel() override;

    Project *project() const;
    void setProject(Project *project);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

signals:
    void projectChanged();

private slots:
    void onPreColourAdded();
    void onPostColourAdded();
    void onColourRenamed(int index);
    void onPreColourRemoved(int index);
    void onPostColourRemoved();
    void onPreSwatchImported();
    void onPostSwatchImported();

private:
    Project *mProject;
};

#endif // SWATCHMODEL_H
