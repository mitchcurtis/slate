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

#ifndef MODIFYANIMATIONCOMMAND_H
#define MODIFYANIMATIONCOMMAND_H

#include <QDebug>
#include <QSize>
#include <QUndoCommand>

#include "slate-global.h"

class Animation;
class AnimationSystem;
class ImageLayer;

class SLATE_EXPORT ModifyAnimationCommand : public QUndoCommand
{
public:
    // TODO:
    // - add Animation *editAnimation to AnimationSystem
    // - have AnimationSettingsPopup set editAnimation's properties to those of currentAnimation and then
    //   currentAnimationPlayback.animation to editAnimation when it opens
    // - modifyAnimation(index) can still be called as usual, as LayeredImageProject can use editAnimation to pass
    //   values to our constructor
    // - have AnimationSettingsPopup set currentAnimationPlayback.animation back to currentAnimation when it closes
    ModifyAnimationCommand(AnimationSystem *animationSystem, int index,
        const QString &name, int fps, int frameCount, int frameX, int frameY, int frameWidth, int frameHeight,
        bool reverse, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;

    int id() const override;

private:
    friend QDebug operator<<(QDebug debug, const ModifyAnimationCommand *command);

    AnimationSystem *mAnimationSystem = nullptr;
    Animation *mAnimation = nullptr;
    int mIndex = -1;

    QString mNewName;
    int mNewFps = 0;
    int mNewFrameCount = 0;
    int mNewFrameX = 0;
    int mNewFrameY = 0;
    int mNewFrameWidth = 0;
    int mNewFrameHeight = 0;
    bool mNewReverse = false;

    QString mOldName;
    int mOldFps = 0;
    int mOldFrameCount = 0;
    int mOldFrameX = 0;
    int mOldFrameY = 0;
    int mOldFrameWidth = 0;
    int mOldFrameHeight = 0;
    bool mOldReverse = false;
};


#endif // MODIFYANIMATIONCOMMAND_H
