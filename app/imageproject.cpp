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

#include "imageproject.h"

#include "changeimagecanvassizecommand.h"
#include "changeimagesizecommand.h"

ImageProject::ImageProject() :
    mUsingAnimation(false)
{
    setObjectName(QLatin1String("imageProject"));
    qCDebug(lcProjectLifecycle) << "constructing" << this;
}

ImageProject::~ImageProject()
{
    qCDebug(lcProjectLifecycle) << "destructing" << this;
}

QSize ImageProject::size() const
{
    return !mImage.isNull() ? mImage.size() : QSize();
}

void ImageProject::setSize(const QSize &newSize)
{
    if (newSize == size())
        return;

    beginMacro(QLatin1String("ChangeCanvasSize"));
    addChange(new ChangeImageCanvasSizeCommand(this, mImage, mImage.copy(0, 0, newSize.width(), newSize.height())));
    endMacro();
}

int ImageProject::widthInPixels() const
{
    return !mImage.isNull() ? mImage.width() : 0;
}

int ImageProject::heightInPixels() const
{
    return !mImage.isNull() ? mImage.height() : 0;
}

void ImageProject::createNew(int imageWidth, int imageHeight, bool transparentBackground)
{
    if (hasLoaded()) {
        close();
    }

    if (mImageUrl.isEmpty()) {
        // Not using an existing image, so we must create one.
        mImageUrl = createTemporaryImage(imageWidth, imageHeight, transparentBackground ? Qt::transparent : Qt::white);
        if (!mImageUrl.isValid()) {
            return;
        }
    }

    qCDebug(lcProject) << "creating new project:"
        << "imageWidth =" << imageWidth
        << "imageHeight =" << imageHeight
        << "imageUrl =" << mImageUrl;

    Q_ASSERT(mUndoStack.count() == 0);
    Q_ASSERT(mImage.isNull());

    mImage = QImage(mImageUrl.toLocalFile());

    setUrl(QUrl());
    setNewProject(true);
    emit projectCreated();

    qCDebug(lcProject) << "finished creating new project";
}

void ImageProject::load(const QUrl &url)
{
    qCDebug(lcProject) << "loading project:" << url;

    mUsingTempImage = false;

    mImage = QImage(url.toLocalFile());
    if (mImage.isNull()) {
        error(QString::fromLatin1("Failed to open project's image at %1").arg(url.toLocalFile()));
        return;
    }

    setUrl(url);
    emit projectLoaded();

    qCDebug(lcProject) << "loaded project";
}

void ImageProject::close()
{
    qCDebug(lcProject) << "closing project...";

    setNewProject(false);
    mImage = QImage();
    mImageUrl = QUrl();
    mUsingTempImage = false;
    setUrl(QUrl());
    mUndoStack.clear();
    mUsingAnimation = false;
    mAnimationPlayback.reset();
    emit projectClosed();

    qCDebug(lcProject) << "... closed project";
}

void ImageProject::saveAs(const QUrl &url)
{
    if (!hasLoaded())
        return;

    if (url.isEmpty())
        return;

    const QString filePath = url.toLocalFile();
    const QFileInfo projectSaveFileInfo(filePath);
    if (mTempDir.isValid()) {
        if (projectSaveFileInfo.dir().path() == mTempDir.path()) {
            error(QLatin1String("Cannot save project in internal temporary directory"));
            return;
        }
    }

    if (mImage.isNull()) {
        error(QString::fromLatin1("Failed to save project: image is null"));
        return;
    }

    if (!mImage.save(filePath)) {
        error(QString::fromLatin1("Failed to save project's image to %1").arg(filePath));
        return;
    }

    mUsingTempImage = false;

    if (mFromNew) {
        // The project was successfully saved, so it can now save
        // to the same URL by default from now on.
        setNewProject(false);
    }
    setUrl(url);
    mUndoStack.setClean();
    mHadUnsavedChangesBeforeMacroBegan = false;
}

void ImageProject::resize(int width, int height, bool smooth)
{
    const QSize newSize(width, height);
    if (newSize == size())
        return;

    beginMacro(QLatin1String("ChangeImageSize"));
    const QImage resized = mImage.scaled(newSize, Qt::IgnoreAspectRatio,
        smooth ? Qt::SmoothTransformation : Qt::FastTransformation);
    addChange(new ChangeImageSizeCommand(this, mImage, resized));
    endMacro();
}

QImage *ImageProject::image()
{
    return &mImage;
}

Project::Type ImageProject::type() const
{
    return ImageType;
}

void ImageProject::doSetCanvasSize(const QImage &newImage)
{
    doSetImageSize(newImage);
}

void ImageProject::doSetImageSize(const QImage &newImage)
{
    if (newImage.width() == 0 || newImage.height() == 0) {
        error(QLatin1String("Cannot set image's width or height to zero"));
        return;
    }

    Q_ASSERT(!mImage.isNull());
    Q_ASSERT(newImage.size() != size());
    mImage = newImage;
    emit sizeChanged();
}

AnimationPlayback *ImageProject::animationPlayback()
{
    return &mAnimationPlayback;
}

QImage ImageProject::exportedImage() const
{
    return mImage;
}

bool ImageProject::isUsingAnimation() const
{
    return mUsingAnimation;
}

void ImageProject::setUsingAnimation(bool usingAnimation)
{
    if (usingAnimation == mUsingAnimation)
        return;

    mUsingAnimation = usingAnimation;
    emit usingAnimationChanged();
}
