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

#ifndef LAYERMODEL_H
#define LAYERMODEL_H

#include <QAbstractListModel>

#include "slate-global.h"

class ImageLayer;
class LayeredImageProject;

class SLATE_EXPORT LayerModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(LayeredImageProject *layeredImageProject READ layeredImageProject WRITE setLayeredImageProject NOTIFY layeredImageProjectChanged)

public:
    enum LayerModelRoles {
        LayerRole = Qt::UserRole,
    };

    explicit LayerModel(QObject *parent = nullptr);

    LayeredImageProject *layeredImageProject() const;
    void setLayeredImageProject(LayeredImageProject *layeredImageProject);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

signals:
    void layeredImageProjectChanged();

public slots:
    void onPreLayersCleared();
    void onPostLayersCleared();
    void onPreLayerAdded(int index);
    void onPostLayerAdded(int index);
    void onPreLayerRemoved(int index);
    void onPostLayerRemoved(int index);
    void onPreLayerMoved(int fromIndex, int toIndex);
    void onPostLayerMoved(int fromIndex, int toIndex);

private:
    LayeredImageProject *mLayeredImageProject;
};

#endif // LAYERMODEL_H
