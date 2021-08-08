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

#include "imageproject.h"

#include "changeimagecanvassizecommand.h"
#include "changeimagesizecommand.h"
#include "imageutils.h"
#include "qtutils.h"
#include "rearrangeimagecontentsintogridcommand.h"

Q_LOGGING_CATEGORY(lcImageProjectLivePreview, "app.imageproject.livepreview")
Q_LOGGING_CATEGORY(lcResize, "app.imageproject.resize")
Q_LOGGING_CATEGORY(lcImageProjectRearrangeContentsIntoGrid, "app.imageproject.rearrangecontentsintogrid")

ImageProject::ImageProject() :
    mAnimationHelper(this, &mAnimationSystem, &mUsingAnimation)
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

QRect ImageProject::bounds() const
{
    const QSize ourSize(size());
    return QRect(0, 0, ourSize.width(), ourSize.height());
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

void ImageProject::beginLivePreview()
{
    qCDebug(lcImageProjectLivePreview) << "beginLivePreview called";

    if (mLivePreviewActive) {
        qWarning() << "Live preview already active";
        return;
    }

    // We could do this stuff lazily (when the first modification is made),
    // but it's better to have any delay from copying happen when
    // the dialog opens instead of as the sliders etc. are being interacted with.

    mImageBeforeLivePreview = mImage;

    mLivePreviewActive = true;

    // Note that mCurrentLivePreviewModification isn't set yet; that happens
    // when the first modification is made.
}

void ImageProject::makeLivePreviewModification(LivePreviewModification modification, const QImage &newImage)
{
    qCDebug(lcImageProjectLivePreview) << "makeLivePreviewModification called with modification" << modification;

    if (warnIfLivePreviewNotActive(QLatin1String("make live preview modification")))
        return;

    if (mCurrentLivePreviewModification == LivePreviewModification::None)
        mCurrentLivePreviewModification = modification;

    if (modification != mCurrentLivePreviewModification) {
        qWarning() << "Cannot make live preview modification" << modification << "as it is different"
            << "to the current modification of" << mCurrentLivePreviewModification;
        return;
    }

    mImage = newImage;

    // Let the canvas know that it should repaint.
    emit contentsModified();
}

void ImageProject::endLivePreview(LivePreviewModificationAction modificationAction)
{
    qCDebug(lcImageProjectLivePreview) << "endLivePreview called with modificationAction" << modificationAction;

    if (!mLivePreviewActive) {
        qWarning() << "Can't end live preview as it isn't active";
        return;
    }

    auto cleanup = [&](){
        mImageBeforeLivePreview = QImage();
        mLivePreviewActive = false;
    };

    if (mCurrentLivePreviewModification == LivePreviewModification::None) {
        // A dialog was opened but no changes were made.
        cleanup();
        return;
    }

    if (modificationAction == CommitModificaton) {
        // The dialog was accepted.
        switch (mCurrentLivePreviewModification) {
        case LivePreviewModification::Resize:
            beginMacro(QLatin1String("ChangeImageSize"));
            addChange(new ChangeImageSizeCommand(this, mImageBeforeLivePreview, mImage));
            endMacro();
            break;
        case LivePreviewModification::RearrangeContentsIntoGrid:
            beginMacro(QLatin1String("RearrangeImageContentsIntoGrid"));
            addChange(new RearrangeImageContentsIntoGridCommand(this, mImageBeforeLivePreview, mImage));
            endMacro();
            break;
        // Image projects currently don't support moving contents.
        case LivePreviewModification::MoveContents:
            qFatal("mCurrentLivePreviewModification is %s, which isn't supported by ImageProject",
                qPrintable(QtUtils::toString(mCurrentLivePreviewModification)));
            break;
        case LivePreviewModification::None:
            qFatal("mCurrentLivePreviewModification is LivePreviewModification::None where it shouldn't be");
            break;
        }
    } else {
        // The dialog was cancelled.
        mImage = mImageBeforeLivePreview;

        // The canvas needs to repaint if the dialog was cancelled, since
        // we're modifying the contents directly.
        emit contentsModified();
    }

    cleanup();
}

void ImageProject::doLoad(const QUrl &url)
{
    mUsingTempImage = false;

    mImage = QImage(url.toLocalFile());
    if (mImage.isNull()) {
        error(QString::fromLatin1("Failed to open project's image at %1").arg(url.toLocalFile()));
        return;
    }

    setUrl(url);
    emit projectLoaded();
}

void ImageProject::doClose()
{
    mImage = QImage();
    mImageUrl = QUrl();
    mUsingTempImage = false;
    mUsingAnimation = false;
    mHasUsedAnimation = false;
    mAnimationSystem.reset();
    emit projectClosed();
}

bool ImageProject::doSaveAs(const QUrl &url)
{
    const QString filePath = url.toLocalFile();
    const QFileInfo projectSaveFileInfo(filePath);
    if (mTempDir.isValid()) {
        if (projectSaveFileInfo.dir().path() == mTempDir.path()) {
            error(QLatin1String("Cannot save project in internal temporary directory"));
            return false;
        }
    }

    if (mImage.isNull()) {
        error(QString::fromLatin1("Failed to save project: image is null"));
        return false;
    }

    if (!mImage.save(filePath)) {
        error(QString::fromLatin1("Failed to save project's image to %1").arg(filePath));
        return false;
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
    return true;
}

void ImageProject::resize(int width, int height, bool smooth)
{
    qCDebug(lcResize) << "resize called with width" << width
        << "height" << height << "smooth" << smooth;

    if (warnIfLivePreviewNotActive(QLatin1String("move contents")))
        return;

    const QSize newSize(width, height);
    if (newSize == size())
        return;

    const QImage resizedImage = ImageUtils::resizeContents(mImage, newSize, smooth);
    makeLivePreviewModification(LivePreviewModification::Resize, resizedImage);
}

void ImageProject::crop(const QRect &rect)
{
    if (rect.x() == 0 && rect.y() == 0 && rect.size() == size()) {
        // No change.
        return;
    }

    beginMacro(QLatin1String("CropImageCanvas"));
    const QImage resized = mImage.copy(rect);
    addChange(new ChangeImageCanvasSizeCommand(this, mImage, resized));
    endMacro();
}

void ImageProject::rearrangeContentsIntoGrid(int cellWidth, int cellHeight, int columns, int rows)
{
    qCDebug(lcImageProjectRearrangeContentsIntoGrid) << "rearrangeContentsIntoGrid called with cellWidth" << cellWidth
        << "cellHeight" << cellHeight << "columns" << columns << "rows" << rows;

    if (warnIfLivePreviewNotActive(QLatin1String("rearrange contents into grid")))
        return;

    const QVector<QImage> newImages = ImageUtils::rearrangeContentsIntoGrid(
        { mImageBeforeLivePreview }, cellWidth, cellHeight, columns, rows);
    makeLivePreviewModification(LivePreviewModification::RearrangeContentsIntoGrid, newImages.first());
}

QImage *ImageProject::image()
{
    return &mImage;
}

Project::Type ImageProject::type() const
{
    return ImageType;
}

void ImageProject::setImage(const QImage &newImage)
{
    if (newImage.width() == 0 || newImage.height() == 0) {
        error(QLatin1String("Cannot set image's width or height to zero"));
        return;
    }

    Q_ASSERT(!mImage.isNull());
    mImage = newImage;
    emit sizeChanged();
}

AnimationSystem *ImageProject::animationSystem()
{
    return &mAnimationSystem;
}

void ImageProject::addAnimation()
{
    mAnimationHelper.addAnimation();
}

void ImageProject::duplicateAnimation(int index)
{
    mAnimationHelper.duplicateAnimation(index);
}

void ImageProject::modifyAnimation(int index)
{
    mAnimationHelper.modifyAnimation(index);
}

void ImageProject::renameAnimation(int index)
{
    mAnimationHelper.renameAnimation(index);
}

void ImageProject::moveCurrentAnimationUp()
{
    mAnimationHelper.moveCurrentAnimationUp();
}

void ImageProject::moveCurrentAnimationDown()
{
    mAnimationHelper.moveCurrentAnimationDown();
}

void ImageProject::removeAnimation(int index)
{
    mAnimationHelper.removeAnimation(index);
}

QImage ImageProject::exportedImage() const
{
    return mImage;
}

void ImageProject::exportGif(const QUrl &url)
{
    if (!mUsingAnimation) {
        // Shouldn't happen, but just in case...
        error(tr("Cannot export as GIF because the project isn't using animation"));
        return;
    }

    QString errorMessage;
    if (!ImageUtils::exportGif(exportedImage(), url, *mAnimationSystem.currentAnimationPlayback(), errorMessage))
        error(errorMessage);
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

    if (mUsingAnimation) {
        if (!mHasUsedAnimation) {
            mAnimationSystem.addNewAnimation(size());

            mHasUsedAnimation = true;
        }
    }

    emit usingAnimationChanged();
}
