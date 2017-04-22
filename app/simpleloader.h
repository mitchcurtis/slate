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

#ifndef SIMPLELOADER_H
#define SIMPLELOADER_H

#include <QQuickItem>

class QQmlComponent;

class SimpleLoader : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(bool active READ isActive WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(QQmlComponent *sourceComponent READ sourceComponent WRITE setSourceComponent RESET resetSourceComponent NOTIFY sourceComponentChanged)
    Q_PROPERTY(QQuickItem *item READ item NOTIFY itemChanged)

public:
    SimpleLoader(QQuickItem *parent = nullptr);
    ~SimpleLoader();

    bool isActive() const;
    void setActive(bool isActive);

    QQmlComponent *sourceComponent() const;
    void setSourceComponent(QQmlComponent *component);
    void resetSourceComponent();

    QQuickItem *item() const;

signals:
    void itemChanged();
    void activeChanged();
    void sourceComponentChanged();
    void loaded();

public slots:


private slots:
    void itemImplicitWidthChanged();
    void itemImplicitHeightChanged();
    void itemGeometryChanged();

private:
    Q_DISABLE_COPY(SimpleLoader)

    void load();
    void clear();
    void updateSize(bool loaderGeometryChanged = true);
    void initResize();

    qreal getImplicitWidth() const;
    qreal getImplicitHeight() const;

    void componentComplete() override;
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;

    bool mActive;
    QQmlComponent *mSourceComponent;
    QQuickItem *mItem;
    bool mUpdatingSize;
};

#endif // SIMPLELOADER_H
