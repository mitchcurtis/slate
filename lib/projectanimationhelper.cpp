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

#include "projectanimationhelper.h"

#include "addanimationcommand.h"
#include "animationsystem.h"
#include "changeanimationordercommand.h"
#include "deleteanimationcommand.h"
#include "duplicateanimationcommand.h"
#include "modifyanimationcommand.h"
#include "project.h"

ProjectAnimationHelper::ProjectAnimationHelper(Project *project, AnimationSystem *animationSystem, const bool *const usingAnimation) :
    mProject(project),
    mAnimationSystem(animationSystem),
    mUsingAnimation(usingAnimation)
{
}

void ProjectAnimationHelper::addAnimation()
{
    if (!mUsingAnimation) {
        qWarning() << "Can't add animations when mUsingAnimation is false";
        return;
    }

    mProject->beginMacro(QLatin1String("AddAnimationCommand"));
    mProject->addChange(new AddAnimationCommand(mAnimationSystem, mProject->size()));
    mProject->endMacro();
}

void ProjectAnimationHelper::duplicateAnimation(int index)
{
    if (!mUsingAnimation) {
        qWarning() << "Can't duplicate animations when mUsingAnimation is false";
        return;
    }

    const auto targetAnimation = mAnimationSystem->animationAt(index);
    const QString duplicateName = mAnimationSystem->generateDuplicateName(targetAnimation);
    if (duplicateName.isEmpty()) {
        qWarning() << "Failed to generate duplicate name for" << targetAnimation->name();
        return;
    }

    mProject->beginMacro(QLatin1String("DuplicateAnimationCommand"));
    mProject->addChange(new DuplicateAnimationCommand(mAnimationSystem, index,
        mAnimationSystem->animationCount(), duplicateName));
    mProject->endMacro();
}

void ProjectAnimationHelper::modifyAnimation(int index)
{
    Animation *animation = mAnimationSystem->animationAt(index);
    if (!animation) {
        qWarning() << "Cannot modify animation" << index << "because it does not exist";
        return;
    }

    const Animation *editAnimation = mAnimationSystem->editAnimation();
    if (*editAnimation == *animation) {
        // Nothing to do, but we don't need to warn about it.
        return;
    }

    mProject->beginMacro(QLatin1String("ModifyAnimationCommand"));
    mProject->addChange(new ModifyAnimationCommand(mAnimationSystem, index, editAnimation->name(),
        editAnimation->fps(), editAnimation->frameCount(), editAnimation->frameX(),
        editAnimation->frameY(), editAnimation->frameWidth(), editAnimation->frameHeight(), editAnimation->isReverse()));
    mProject->endMacro();
}

void ProjectAnimationHelper::renameAnimation(int index)
{
    Animation *animation = mAnimationSystem->animationAt(index);
    if (!animation) {
        qWarning() << "Cannot rename animation" << index << "because it does not exist";
        return;
    }

    const Animation *editAnimation = mAnimationSystem->editAnimation();
    if (*editAnimation == *animation) {
        // Nothing to do, but we don't need to warn about it.
        return;
    }

    mProject->beginMacro(QLatin1String("ModifyAnimationCommand"));
    mProject->addChange(new ModifyAnimationCommand(mAnimationSystem, index, editAnimation->name(),
        animation->fps(), animation->frameCount(), animation->frameX(),
        animation->frameY(), animation->frameWidth(), animation->frameHeight(), animation->isReverse()));
    mProject->endMacro();
}

void ProjectAnimationHelper::moveCurrentAnimationUp()
{
    const int currentIndex = mAnimationSystem->currentAnimationIndex();
    if (currentIndex <= 0) {
        qWarning() << "Cannot move current animation up as it's already at the top";
        return;
    }

    mProject->beginMacro(QLatin1String("ChangeAnimationOrderCommand"));
    mProject->addChange(new ChangeAnimationOrderCommand(mAnimationSystem, currentIndex, currentIndex - 1));
    mProject->endMacro();
}

void ProjectAnimationHelper::moveCurrentAnimationDown()
{
    const int currentIndex = mAnimationSystem->currentAnimationIndex();
    if (currentIndex >= mAnimationSystem->animationCount() - 1) {
        qWarning() << "Cannot move current animation down as it's already at the bottom";
        return;
    }

    mProject->beginMacro(QLatin1String("ChangeAnimationOrderCommand"));
    mProject->addChange(new ChangeAnimationOrderCommand(mAnimationSystem, currentIndex, currentIndex + 1));
    mProject->endMacro();
}

void ProjectAnimationHelper::removeAnimation(int index)
{
    const Animation *animation = mAnimationSystem->animationAt(index);
    if (!animation) {
        qWarning() << "Cannot remove animation at index" << index << "because it does not exist";
        return;
    }

    mProject->beginMacro(QLatin1String("RemoveAnimationCommand"));
    mProject->addChange(new DeleteAnimationCommand(mAnimationSystem, index));
    mProject->endMacro();
}
