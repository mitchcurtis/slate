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

#ifndef AUTOSWATCHMODEL_H
#define AUTOSWATCHMODEL_H

#include <QAbstractListModel>
#include <QColor>
#include <QVector>

class ImageLayer;
class ImageCanvas;

class AutoSwatchModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(ImageCanvas *canvas READ canvas WRITE setCanvas NOTIFY canvasChanged)

public:
    enum {
        ColourRole = Qt::UserRole,
    };

    explicit AutoSwatchModel(QObject *parent = nullptr);

    ImageCanvas *canvas() const;
    void setCanvas(ImageCanvas *canvas);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

signals:
    void canvasChanged();

private slots:
    void onProjectChanged();
    void onUndoStackIndexChanged();

private:
    ImageCanvas *mCanvas;
    QVector<QColor> mColours;
};

#endif // AUTOSWATCHMODEL_H
