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

#include "clipboard.h"

#include <QClipboard>
#include <QGuiApplication>
#include <QImage>

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
    // Don't need to worry about change signals, as the image is accessed
    // imperatively and we don't care about changes.
    mImage = image;
}

Clipboard::Clipboard(QObject *parent) :
    QObject(parent),
    mClipboardImage(new ClipboardImage(qGuiApp->clipboard()->image(), this))
{
    QObject::connect(qGuiApp->clipboard(), &QClipboard::changed, [=]() {
        mClipboardImage->setImage(qGuiApp->clipboard()->image());
    });
}

ClipboardImage *Clipboard::image() const
{
    return mClipboardImage;
}
