/*
    Copyright 2023, Mitch Curtis

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

#include "imagelayer.h"

#include <QBuffer>
#include <QJsonObject>

ImageLayer::ImageLayer()
{
}

ImageLayer::ImageLayer(QObject *parent, const QImage &image) :
    QObject(parent),
    mVisible(true),
    mOpacity(1.0),
    mImage(image)
{
}

ImageLayer::~ImageLayer()
{
}

QString ImageLayer::name() const
{
    return mName;
}

void ImageLayer::setName(const QString &name)
{
    if (name == mName)
        return;

    mName = name;
    setObjectName(mName);
    emit nameChanged();
}

QSize ImageLayer::size() const
{
    return !mImage.isNull() ? mImage.size() : QSize();
}

void ImageLayer::setSize(const QSize &newSize)
{
    if (newSize == size())
        return;

    mImage = mImage.copy(0, 0, newSize.width(), newSize.height());
}

QImage *ImageLayer::image()
{
    return &mImage;
}

const QImage *ImageLayer::image() const
{
    return &mImage;
}

qreal ImageLayer::opacity() const
{
    return mOpacity;
}

void ImageLayer::setOpacity(const qreal &opacity)
{
    if (qFuzzyCompare(opacity, mOpacity))
        return;

    mOpacity = opacity;
    emit opacityChanged();
}

bool ImageLayer::isVisible() const
{
    return mVisible;
}

void ImageLayer::setVisible(bool visible)
{
    if (visible == mVisible)
        return;

    mVisible = visible;
    emit visibleChanged();
}

ImageLayer *ImageLayer::clone()
{
    ImageLayer *layer = new ImageLayer;
    layer->setName(mName + QLatin1String(" copy"));
    layer->setVisible(mVisible);
    layer->setOpacity(mOpacity);
    layer->mImage = mImage;
    return layer;
}

void ImageLayer::read(const QJsonObject &jsonObject)
{
    setName(jsonObject.value("name").toString());
    setOpacity(jsonObject.value("opacity").toDouble());
    setVisible(jsonObject.value("visible").toBool());

    const QString base64ImageData = jsonObject.value("imageData").toString();
    QByteArray imageData = QByteArray::fromBase64(base64ImageData.toLatin1());
    mImage.loadFromData(imageData, "png");
}

void ImageLayer::write(QJsonObject &jsonObject)
{
    jsonObject["name"] = mName;
    jsonObject["opacity"] = mOpacity;
    jsonObject["visible"] = mVisible;

    QByteArray imageData;
    QBuffer buffer { &imageData };
    buffer.open(QIODevice::WriteOnly);
    mImage.save(&buffer, "png");
    const QByteArray base64ImageData = buffer.data().toBase64();
    jsonObject["imageData"] = QString::fromLatin1(base64ImageData);
}
