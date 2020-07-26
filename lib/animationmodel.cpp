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

#include "animationmodel.h"

#include <QLoggingCategory>

#include "imagelayer.h"
#include "layeredimageproject.h"

Q_LOGGING_CATEGORY(lcAnimationModel, "app.animationModel")

AnimationModel::AnimationModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

AnimationSystem *AnimationModel::animationSystem() const
{
    return mAnimationSystem;
}

void AnimationModel::setAnimationSystem(AnimationSystem *animationSystem)
{
    if (animationSystem == mAnimationSystem)
        return;

    if (mAnimationSystem)
        mAnimationSystem->disconnect(this);

    beginResetModel();
    mAnimationSystem = animationSystem;
    endResetModel();
    emit animationSystemChanged();

    if (mAnimationSystem) {
        connect(mAnimationSystem, &AnimationSystem::preAnimationAdded, this, &AnimationModel::onPreAnimationAdded);
        connect(mAnimationSystem, &AnimationSystem::postAnimationAdded, this, &AnimationModel::onPostAnimationAdded);
        connect(mAnimationSystem, &AnimationSystem::animationModified, this, &AnimationModel::onAnimationModified);
        connect(mAnimationSystem, &AnimationSystem::preAnimationMoved, this, &AnimationModel::onPreAnimationMoved);
        connect(mAnimationSystem, &AnimationSystem::postAnimationMoved, this, &AnimationModel::onPostAnimationMoved);
        connect(mAnimationSystem, &AnimationSystem::preAnimationRemoved, this, &AnimationModel::onPreAnimationRemoved);
        connect(mAnimationSystem, &AnimationSystem::postAnimationRemoved, this, &AnimationModel::onPostAnimationRemoved);
    }
}

QVariant AnimationModel::data(const QModelIndex &index, int role) const
{
    if (!mAnimationSystem || !checkIndex(index, CheckIndexOption::IndexIsValid))
        return QVariant();

    Animation *animation = mAnimationSystem->animationAt(index.row());
    if (!animation)
        return QVariant();

    if (role == AnimationRole)
        return QVariant::fromValue(animation);

    return QVariant();
}

int AnimationModel::rowCount(const QModelIndex &) const
{
    if (!mAnimationSystem)
        return 0;

    return mAnimationSystem->animationCount();
}

int AnimationModel::columnCount(const QModelIndex &) const
{
    return 1;
}

QHash<int, QByteArray> AnimationModel::roleNames() const
{
    QHash<int, QByteArray> names;
    names.insert(AnimationRole, "animation");
    return names;
}

void AnimationModel::onPreAnimationAdded(int index)
{
    qCDebug(lcAnimationModel) << "pre-add at index" << index;
    beginInsertRows(QModelIndex(), index, index);
}

void AnimationModel::onPostAnimationAdded(int index)
{
    qCDebug(lcAnimationModel) << "post-add at index" << index;
    endInsertRows();
}

void AnimationModel::onAnimationModified(int index)
{
    const auto modelIndex = createIndex(index, 0);
    dataChanged(modelIndex, modelIndex);
}

void AnimationModel::onPreAnimationRemoved(int index)
{
    qCDebug(lcAnimationModel) << "pre-remove at index" << index;
    beginRemoveRows(QModelIndex(), index, index);
}

void AnimationModel::onPostAnimationRemoved(int index)
{
    qCDebug(lcAnimationModel) << "post-remove at index" << index;
    endRemoveRows();
}

void AnimationModel::onPreAnimationMoved(int fromIndex, int toIndex)
{
    const int actualToIndex = toIndex > fromIndex ? toIndex + 1 : toIndex;
    if (!beginMoveRows(QModelIndex(), fromIndex, fromIndex, QModelIndex(), actualToIndex)) {
        qWarning() << "beginMoveRows() failed when trying to move" << fromIndex << "to" << toIndex
                   << "- weird stuff might be about to happen since we're not going to cancel the move operation";
    }
}

void AnimationModel::onPostAnimationMoved(int, int)
{
    endMoveRows();
}
