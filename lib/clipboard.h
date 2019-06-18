/*
    Copyright 2019, Mitch Curtis

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

#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include <QImage>
#include <QObject>

#include "slate-global.h"

class SLATE_EXPORT ClipboardImage : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int width READ width CONSTANT FINAL)
    Q_PROPERTY(int height READ height CONSTANT FINAL)

public:
    ClipboardImage(const QImage &image, QObject *parent = nullptr);

    int width() const;
    int height() const;

    void setImage(const QImage &image);

private:
    QImage mImage;
};

class SLATE_EXPORT Clipboard : public QObject
{
    Q_OBJECT

public:
    explicit Clipboard(QObject *parent = nullptr);

    Q_INVOKABLE ClipboardImage *image() const;

private:
    ClipboardImage *mClipboardImage;
};

#endif // CLIPBOARD_H
