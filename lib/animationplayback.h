/*
    Copyright 2018, Mitch Curtis

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

#ifndef ANIMATIONPLAYBACK_H
#define ANIMATIONPLAYBACK_H

#include <QObject>
#include <QPoint>
#include <QSize>

#include "slate-global.h"

class QJsonObject;

class SLATE_EXPORT AnimationPlayback : public QObject
{
    Q_OBJECT
    // Serialised.
    Q_PROPERTY(int fps READ fps WRITE setFps NOTIFY fpsChanged FINAL)
    Q_PROPERTY(int frameCount READ frameCount WRITE setFrameCount NOTIFY frameCountChanged FINAL)
    Q_PROPERTY(int frameX READ frameX WRITE setFrameX NOTIFY frameXChanged)
    Q_PROPERTY(int frameY READ frameY WRITE setFrameY NOTIFY frameYChanged)
    Q_PROPERTY(int frameWidth READ frameWidth WRITE setFrameWidth NOTIFY frameWidthChanged FINAL)
    Q_PROPERTY(int frameHeight READ frameHeight WRITE setFrameHeight NOTIFY frameHeightChanged FINAL)
    Q_PROPERTY(int currentFrameIndex READ currentFrameIndex NOTIFY currentFrameIndexChanged FINAL)
    Q_PROPERTY(qreal scale READ scale WRITE setScale NOTIFY scaleChanged FINAL)
    Q_PROPERTY(bool loop READ shouldLoop WRITE setLoop NOTIFY loopChanged)

    // Not serialised.
    Q_PROPERTY(bool playing READ isPlaying WRITE setPlaying NOTIFY playingChanged)

public:
    explicit AnimationPlayback(QObject *parent = nullptr);

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

    int currentFrameIndex() const;

    qreal scale() const;
    void setScale(const qreal &scale);

    bool isPlaying() const;
    void setPlaying(bool playing);

    bool shouldLoop() const;
    void setLoop(bool shouldLoop);

    int startColumn() const;
    int startRow() const;
    int startIndex(int sourceImageWidth) const;

    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

    void reset();

signals:
    void fpsChanged();
    void frameCountChanged();
    void frameXChanged();
    void frameYChanged();
    void frameWidthChanged();
    void frameHeightChanged();
    void currentFrameIndexChanged();
    void scaleChanged();
    void loopChanged();
    void playingChanged();

private:
    void setCurrentFrameIndex(int currentFrameIndex);

    void timerEvent(QTimerEvent *event) override;

    int mFps;
    int mFrameCount;
    int mFrameX;
    int mFrameY;
    int mFrameWidth;
    int mFrameHeight;
    int mCurrentFrameIndex;
    qreal mScale;
    bool mPlaying;
    bool mLoop;

    int mTimerId;
};

#endif // ANIMATIONPLAYBACK_H
