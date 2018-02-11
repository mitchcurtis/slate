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

class QJsonObject;

class AnimationPlayback : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int fps READ fps WRITE setFps NOTIFY fpsChanged FINAL)
    Q_PROPERTY(int frameCount READ frameCount WRITE setFrameCount NOTIFY frameCountChanged FINAL)
    Q_PROPERTY(int frameWidth READ frameWidth WRITE setFrameWidth NOTIFY frameWidthChanged FINAL)
    Q_PROPERTY(int frameHeight READ frameHeight WRITE setFrameHeight NOTIFY frameHeightChanged FINAL)
    Q_PROPERTY(int currentFrameIndex READ currentFrameIndex NOTIFY currentFrameIndexChanged FINAL)

    Q_PROPERTY(bool playing READ isPlaying WRITE setPlaying NOTIFY playingChanged)

public:
    explicit AnimationPlayback(QObject *parent = 0);

    int fps() const;
    void setFps(int fps);

    int frameCount() const;
    void setFrameCount(int frameCount);

    int frameWidth() const;
    void setFrameWidth(int frameWidth);

    int frameHeight() const;
    void setFrameHeight(int frameHeight);

    int currentFrameIndex() const;

    bool isPlaying() const;
    void setPlaying(bool playing);

    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

    void reset();

signals:
    void fpsChanged();
    void frameCountChanged();
    void frameWidthChanged();
    void frameHeightChanged();
    void currentFrameIndexChanged();
    void playingChanged();

private:
    void setCurrentFrameIndex(int currentFrameIndex);

    void timerEvent(QTimerEvent *event) override;

    int mFps;
    int mFrameCount;
    int mFrameWidth;
    int mFrameHeight;
    int mCurrentFrameIndex;
    bool mPlaying;

    int mTimerId;
};

#endif // ANIMATIONPLAYBACK_H
