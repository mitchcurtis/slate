/*
    Copyright 2017, Mitch Curtis

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

#include "simpleloader.h"

#include <QQmlComponent>

SimpleLoader::SimpleLoader(QQuickItem *parent) :
    QQuickItem(parent),
    mActive(true),
    mSourceComponent(0),
    mItem(nullptr),
    mUpdatingSize(false)
{
}

SimpleLoader::~SimpleLoader()
{
}

bool SimpleLoader::isActive() const
{
    return mActive;
}

void SimpleLoader::setActive(bool active)
{
    if (mActive == active)
        return;

    mActive = active;
    if (mActive) {
        load();
    } else {
        if (mItem) {
            clear();
        }
    }
    emit activeChanged();
}

QQmlComponent *SimpleLoader::sourceComponent() const
{
    return mSourceComponent;
}

void SimpleLoader::setSourceComponent(QQmlComponent *component)
{
    if (component == mSourceComponent)
        return;

    if (mSourceComponent)
        clear();

    mSourceComponent = component;

    if (mSourceComponent)
        load();

    emit sourceComponentChanged();
}

void SimpleLoader::resetSourceComponent()
{
    setSourceComponent(nullptr);
}

QQuickItem *SimpleLoader::item() const
{
    return mItem;
}

void SimpleLoader::itemImplicitWidthChanged()
{
    setImplicitWidth(getImplicitWidth());
}

void SimpleLoader::itemImplicitHeightChanged()
{
    setImplicitHeight(getImplicitHeight());
}

void SimpleLoader::itemGeometryChanged()
{
    updateSize(false);
}

void SimpleLoader::load()
{
    if (!isComponentComplete())
        return;

    QQuickItem *newItem = qobject_cast<QQuickItem*>(mSourceComponent->create(qmlContext(this)));
    if (newItem) {
        mItem = newItem;
        connect(mItem, &QQuickItem::widthChanged, this, &SimpleLoader::itemGeometryChanged);
        connect(mItem, &QQuickItem::heightChanged, this, &SimpleLoader::itemGeometryChanged);
        connect(mItem, &QQuickItem::implicitWidthChanged, this, &SimpleLoader::itemImplicitWidthChanged);
        connect(mItem, &QQuickItem::implicitHeightChanged, this, &SimpleLoader::itemImplicitHeightChanged);
        newItem->setParent(this);
        newItem->setParentItem(this);
        initResize();
        emit itemChanged();
        emit loaded();
    }
}

void SimpleLoader::clear()
{
    if (mItem) {
        mItem->setParentItem(0);
        mItem->setVisible(false);
        delete mItem;
        mItem = 0;
        emit itemChanged();
    }
}

void SimpleLoader::updateSize(bool loaderGeometryChanged)
{
    if (!mItem)
        return;

    if (loaderGeometryChanged && widthValid())
        mItem->setWidth(width());
    if (loaderGeometryChanged && heightValid())
        mItem->setHeight(height());

    if (mUpdatingSize)
        return;

    mUpdatingSize = true;

    setImplicitSize(getImplicitWidth(), getImplicitHeight());

    mUpdatingSize = false;
}

void SimpleLoader::initResize()
{
    if (!mItem)
        return;
    updateSize();
}

qreal SimpleLoader::getImplicitWidth() const
{
    return widthValid() ? mItem->implicitWidth() : mItem->width();
}

qreal SimpleLoader::getImplicitHeight() const
{
    return heightValid() ? mItem->implicitHeight() : mItem->height();
}

void SimpleLoader::componentComplete()
{
    QQuickItem::componentComplete();
    if (mActive && mSourceComponent)
        load();
}

void SimpleLoader::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    if (newGeometry != oldGeometry) {
        updateSize();
    }
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
}
