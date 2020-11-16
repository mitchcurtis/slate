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

#ifndef ANIMATIONPLAYBACK_H
#define ANIMATIONPLAYBACK_H

#include <QObject>
#include <QPoint>
#include <QSize>

#include "slate-global.h"

class QJsonObject;

class Animation;

class SLATE_EXPORT AnimationPlayback : public QObject
{
    Q_OBJECT
    // Serialised.
    Q_PROPERTY(int currentFrameIndex READ currentFrameIndex NOTIFY currentFrameIndexChanged FINAL)
    Q_PROPERTY(qreal scale READ scale WRITE setScale NOTIFY scaleChanged FINAL)
    Q_PROPERTY(bool loop READ shouldLoop WRITE setLoop NOTIFY loopChanged)

    // Not serialised.
    Q_PROPERTY(Animation *animation READ animation WRITE setAnimation NOTIFY animationChanged)
    Q_PROPERTY(bool playing READ isPlaying WRITE setPlaying NOTIFY playingChanged)
    Q_PROPERTY(qreal progress READ progress NOTIFY currentFrameIndexChanged FINAL)

public:
    explicit AnimationPlayback(QObject *parent = nullptr);

    Animation *animation() const;
    void setAnimation(Animation *animation);

    int currentFrameIndex() const;
    qreal progress() const;

    qreal scale() const;
    void setScale(const qreal &scale);

    bool isPlaying() const;
    void setPlaying(bool playing);

    bool shouldLoop() const;
    void setLoop(bool shouldLoop);

    int pauseIndex() const;

    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

    void reset();

signals:
    void animationChanged(Animation *oldAnimation);
    void currentFrameIndexChanged();
    void scaleChanged();
    void loopChanged();
    void playingChanged();

private slots:
    void onFpsChanged();
    void setCurrentIndexToStart();

private:
    int startFrameIndex() const;

    void setCurrentFrameIndex(int currentFrameIndex);

    void timerEvent(QTimerEvent *event) override;

    Animation *mAnimation = nullptr;

    int mCurrentFrameIndex = -1;
    int mPauseIndex = -1;
    qreal mScale = 0.0;
    bool mPlaying = false;
    // Store this so that we can resume playing if the animation is changed,
    // which is convenient because it allows us to switch between playing animations quickly.
    bool mWasPlayingBeforeAnimationChanged = false;
    bool mLoop = false;

    int mTimerId = -1;
};

#endif // ANIMATIONPLAYBACK_H
