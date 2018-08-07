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

#ifndef IMAGELAYER_H
#define IMAGELAYER_H

#include <QImage>
#include <QObject>

class QJsonObject;

class ImageLayer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(qreal opacity READ opacity NOTIFY opacityChanged)
    Q_PROPERTY(bool visible READ isVisible NOTIFY visibleChanged)

public:
    ImageLayer();
    explicit ImageLayer(QObject *parent, const QImage &image = QImage());
    ~ImageLayer();

    QString name() const;
    void setName(const QString &name);

    QImage *image();
    const QImage *image() const;

    QSize size() const;
    void setSize(const QSize &newSize);

    qreal opacity() const;
    void setOpacity(const qreal &opacity);

    bool isVisible() const;
    void setVisible(bool visible);

    ImageLayer *clone();

    void read(const QJsonObject &jsonObject);
    void write(QJsonObject &jsonObject);

signals:
    void nameChanged();
    void opacityChanged();
    void visibleChanged();

private:
    QString mName;
    bool mVisible = false;
    qreal mOpacity = 0.0;
    QImage mImage;
};

#endif // IMAGELAYER_H
