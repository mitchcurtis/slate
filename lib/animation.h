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

#ifndef ANIMATION_H
#define ANIMATION_H

#include <QDebug>
#include <QObject>
#include <QPoint>
#include <QSize>

#include "slate-global.h"

class QJsonObject;

class SLATE_EXPORT Animation : public QObject
{
    Q_OBJECT
    // Serialised.
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged FINAL)
    Q_PROPERTY(int fps READ fps WRITE setFps NOTIFY fpsChanged FINAL)
    Q_PROPERTY(int frameCount READ frameCount WRITE setFrameCount NOTIFY frameCountChanged FINAL)
    Q_PROPERTY(int frameX READ frameX WRITE setFrameX NOTIFY frameXChanged)
    Q_PROPERTY(int frameY READ frameY WRITE setFrameY NOTIFY frameYChanged)
    Q_PROPERTY(int frameWidth READ frameWidth WRITE setFrameWidth NOTIFY frameWidthChanged FINAL)
    Q_PROPERTY(int frameHeight READ frameHeight WRITE setFrameHeight NOTIFY frameHeightChanged FINAL)
    Q_PROPERTY(bool reverse READ isReverse WRITE setReverse NOTIFY reverseChanged)

public:
    explicit Animation(QObject *parent = nullptr);

    QString name() const;
    void setName(const QString &name);

    int fps() const;
    void setFps(int fps);

    int frameCount() const;
    void setFrameCount(int frameCount);

    int frameX() const;
    void setFrameX(int frameX);

    int frameY() const;
    void setFrameY(int frameY);

    int frameWidth() const;
    void setFrameWidth(int frameWidth);
    int framesWide(int sourceImageWidth) const;

    int frameHeight() const;
    void setFrameHeight(int frameHeight);

    bool isReverse() const;
    void setReverse(bool reverse);

    int startColumn() const;
    int startRow() const;
    Q_INVOKABLE int startIndex(int sourceImageWidth) const;

    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

signals:
    void nameChanged();
    void fpsChanged();
    void frameCountChanged();
    void frameXChanged();
    void frameYChanged();
    void frameWidthChanged();
    void frameHeightChanged();
    void reverseChanged();

private:
    QString mName;
    int mFps = 0;
    int mFrameCount = 0;
    int mFrameX = 0;
    int mFrameY = 0;
    int mFrameWidth = 0;
    int mFrameHeight = 0;
    bool mReverse = false;
};

SLATE_EXPORT bool operator==(const Animation &lhs, const Animation &rhs);

SLATE_EXPORT QDebug operator<<(QDebug debug, const Animation *animation);

#endif // ANIMATION_H
