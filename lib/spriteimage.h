/*
    Copyright 2016, Mitch Curtis

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

#ifndef SPRITEIMAGE_H
#define SPRITEIMAGE_H

#include <QQuickPaintedItem>

#include "slate-global.h"

class AnimationPlayback;
class Project;

class SLATE_EXPORT SpriteImage : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(Project *project READ project WRITE setProject NOTIFY projectChanged)
    Q_PROPERTY(AnimationPlayback *animationPlayback READ animationPlayback WRITE setAnimationPlayback NOTIFY animationPlaybackChanged)

public:
    SpriteImage();

    void paint(QPainter *painter) override;

    Project *project() const;
    void setProject(Project *project);

    AnimationPlayback *animationPlayback() const;
    void setAnimationPlayback(AnimationPlayback *animationPlayback);

signals:
    void projectChanged();
    void animationPlaybackChanged();

private slots:
    void onFrameSizeChanged();

private:
    Project *mProject;
    AnimationPlayback *mAnimationPlayback;
};

#endif // SPRITEIMAGE_H
