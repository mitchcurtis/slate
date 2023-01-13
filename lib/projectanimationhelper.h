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

#ifndef PROJECTANIMATIONHELPER_H
#define PROJECTANIMATIONHELPER_H

#include "slate-global.h"

class AnimationSystem;
class Project;

/*!
    Convenience class to reuse code that would otherwise be duplicated between
    LayeredImageProject and ImageProject, since animation is not supported by all project types.
*/
class SLATE_EXPORT ProjectAnimationHelper
{
public:
    ProjectAnimationHelper(Project *project, AnimationSystem *animationSystem, const bool *const usingAnimation);

    void addAnimation();
    void duplicateAnimation(int index);
    void modifyAnimation(int index);
    void renameAnimation(int index);
    void moveCurrentAnimationUp();
    void moveCurrentAnimationDown();
    void removeAnimation(int index);

private:
    Project *mProject;
    AnimationSystem *mAnimationSystem;
    const bool *const mUsingAnimation;
};

#endif // PROJECTANIMATIONHELPER_H
