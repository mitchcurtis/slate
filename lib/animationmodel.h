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

#ifndef ANIMATIONMODEL_H
#define ANIMATIONMODEL_H

#include <QAbstractListModel>
#include <QQmlEngine>

#include "slate-global.h"

class AnimationSystem;

class SLATE_EXPORT AnimationModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(AnimationSystem *animationSystem READ animationSystem WRITE setAnimationSystem NOTIFY animationSystemChanged)
    QML_ELEMENT
    Q_MOC_INCLUDE("animationsystem.h")

public:
    enum {
        AnimationRole = Qt::UserRole,
    };

    explicit AnimationModel(QObject *parent = nullptr);

    AnimationSystem *animationSystem() const;
    void setAnimationSystem(AnimationSystem *animationSystem);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

signals:
    void animationSystemChanged();

public slots:
    void onPreAnimationAdded(int index);
    void onPostAnimationAdded(int index);
    void onAnimationModified(int index);
    void onPreAnimationRemoved(int index);
    void onPostAnimationRemoved(int index);
    void onPreAnimationMoved(int fromIndex, int toIndex);
    void onPostAnimationMoved(int fromIndex, int toIndex);

private:
    AnimationSystem *mAnimationSystem = nullptr;
};

#endif // ANIMATIONMODEL_H
