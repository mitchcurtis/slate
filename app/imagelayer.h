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

class ImageLayer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
//    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity NOTIFY opacityChanged)

public:
    ImageLayer();
    explicit ImageLayer(QObject *parent, const QImage &image);
    ~ImageLayer();

    QString name() const;
    void setName(const QString &name);

    QImage *image();
    const QImage *image() const;

    QSize size() const;
    void setSize(const QSize &newSize);

    qreal opacity() const;
    void setOpacity(const qreal &opacity);

signals:
    void nameChanged();
    void opacityChanged();

private:
    QString mName;
    QImage mImage;
    qreal mOpacity;
};

#endif // IMAGELAYER_H
