#include "guidemodel.h"

#include "guide.h"
#include "project.h"

GuideModel::GuideModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

Project *GuideModel::project() const
{
    return mProject;
}

void GuideModel::setProject(Project *project)
{
    if (project == mProject)
        return;

    if (mProject)
        mProject->disconnect(this);

    beginResetModel();

    mProject = project;

    if (mProject) {
        connect(mProject, &Project::preGuideAppended, this, &GuideModel::onPreGuideAppended);
        connect(mProject, &Project::postGuideAppended, this, &GuideModel::onPostGuideAppended);
        connect(mProject, &Project::guidePositionChanged, this, &GuideModel::onGuidePositionChanged);
        connect(mProject, &Project::preGuideRemoved, this, &GuideModel::onPreGuideRemoved);
        connect(mProject, &Project::postGuideRemoved, this, &GuideModel::onPostGuideRemoved);
    }

    endResetModel();

    emit projectChanged();
}

int GuideModel::rowCount(const QModelIndex &parent) const
{
    if (!mProject)
        return 0;

    return mProject->guides().size();
}

int GuideModel::columnCount(const QModelIndex &parent) const
{
    if (!mProject)
        return 0;

    return 1;
}

QVariant GuideModel::data(const QModelIndex &index, int role) const
{
    if (!checkIndex(index))
        return QVariant();

    const Guide guide = mProject->guides().at(index.row());

    switch (role) {
    case XPositionRole:
        return QVariant(guide.orientation() == Qt::Horizontal ? 0 : guide.position());
        break;
    case YPositionRole:
        return QVariant(guide.orientation() == Qt::Horizontal ? guide.position() : 0);
        break;
    case OrientationRole:
        return QVariant::fromValue(guide.orientation());
        break;
    }

    return QVariant();
}

QHash<int, QByteArray> GuideModel::roleNames() const
{
    QHash<int, QByteArray> names;
    names[XPositionRole] = "xPosition";
    names[YPositionRole] = "yPosition";
    names[OrientationRole] = "orientation";
    return names;
}

void GuideModel::onPreGuideAppended()
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
}

void GuideModel::onPostGuideAppended()
{
    endInsertRows();
}

void GuideModel::onGuidePositionChanged(int index)
{
    const QModelIndex modelIndex(createIndex(index, 0));
    emit dataChanged(modelIndex, modelIndex);
}

void GuideModel::onPreGuideRemoved(int index)
{
    emit beginRemoveRows(QModelIndex(), index, index);
}

void GuideModel::onPostGuideRemoved()
{
    endRemoveRows();
}
