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

#include "clipboard.h"

#include <QClipboard>
#include <QGuiApplication>
#include <QImage>
#include <QQmlEngine>

#include "imagelayer.h"

Q_GLOBAL_STATIC(Clipboard, clipboardInstance)

ClipboardImage::ClipboardImage(const QImage &image, QObject *parent) :
    QObject(parent),
    mImage(image)
{
}

int ClipboardImage::width() const
{
    return mImage.width();
}

int ClipboardImage::height() const
{
    return mImage.height();
}

void ClipboardImage::setImage(const QImage &image)
{
    mImage = image;
}

Clipboard::Clipboard(QObject *parent) :
    QObject(parent),
    mClipboardImage(new ClipboardImage(qGuiApp->clipboard()->image(), this))
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    QObject::connect(qGuiApp->clipboard(), &QClipboard::changed, this, [=]() {
        mClipboardImage->setImage(qGuiApp->clipboard()->image());
    });
}

ClipboardImage *Clipboard::image() const
{
    return mClipboardImage;
}

QVector<QImage> Clipboard::copiedLayerImages() const
{
    return mCopiedLayers;
}

int Clipboard::copiedLayerCount() const
{
    return mCopiedLayers.size();
}

void Clipboard::setCopiedLayerImages(const QVector<QImage> &copiedLayers)
{
    mCopiedLayers = copiedLayers;
    emit copiedLayersChanged();
}

Clipboard *Clipboard::instance()
{
    return clipboardInstance();
}

QObject *Clipboard::qmlInstance(QQmlEngine *, QJSEngine *)
{
    return Clipboard::instance();
}
