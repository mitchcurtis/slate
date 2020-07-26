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

#ifndef LAYEREDIMAGEPROJECT_H
#define LAYEREDIMAGEPROJECT_H

#include <QDebug>
#include <QImage>

#include "animationsystem.h"
#include "project.h"
#include "slate-global.h"

class ImageLayer;

class SLATE_EXPORT LayeredImageProject : public Project
{
    Q_OBJECT
    Q_PROPERTY(int currentLayerIndex READ currentLayerIndex WRITE setCurrentLayerIndex NOTIFY currentLayerIndexChanged)
    Q_PROPERTY(ImageLayer *currentLayer READ currentLayer NOTIFY postCurrentLayerChanged)
    Q_PROPERTY(int layerCount READ layerCount NOTIFY layerCountChanged)
    Q_PROPERTY(bool autoExportEnabled READ isAutoExportEnabled WRITE setAutoExportEnabled NOTIFY autoExportEnabledChanged)
    Q_PROPERTY(bool usingAnimation READ isUsingAnimation WRITE setUsingAnimation NOTIFY usingAnimationChanged)
    Q_PROPERTY(AnimationSystem *animationSystem READ animationSystem CONSTANT FINAL)

public:
    LayeredImageProject();
    ~LayeredImageProject() override;

    ImageLayer *currentLayer();
    int currentLayerIndex() const override;
    void setCurrentLayerIndex(int index);
    ImageLayer *layerAt(int index);
    const ImageLayer *layerAt(int index) const;
    const ImageLayer *layerAt(const QString &name) const;
    int layerCount() const;

    Type type() const override;
    QSize size() const override;
    void setSize(const QSize &newSize) override;
    int widthInPixels() const override;
    int heightInPixels() const override;
    QRect bounds() const override;

    QImage flattenedImage(const std::function<QImage(int)> &layerSubstituteFunction = nullptr) const;
    QImage flattenedImage(int fromIndex, int toIndex, const std::function<QImage(int)> &layerSubstituteFunction = nullptr) const;
    QHash<QString, QImage> flattenedImages() const;
    QImage exportedImage() const override;

    bool isAutoExportEnabled() const;
    void setAutoExportEnabled(bool autoExportEnabled);
    static QString autoExportFilePath(const QUrl &projectUrl);

    bool isUsingAnimation() const;
    void setUsingAnimation(bool isUsingAnimation);

    AnimationSystem *animationSystem();

    Q_INVOKABLE void exportGif(const QUrl &url);

signals:
    void currentLayerIndexChanged();
    void preCurrentLayerChanged();
    void postCurrentLayerChanged();
    void layerCountChanged();
    void autoExportEnabledChanged();
    void usingAnimationChanged();

    void preLayersCleared();
    void postLayersCleared();
    void preLayerAdded(int index);
    void postLayerAdded(int index);
    void preLayerRemoved(int index);
    void postLayerRemoved(int index);
    void preLayerMoved(int fromIndex, int toIndex);
    void postLayerMoved(int fromIndex, int toIndex);
    void postLayerImageChanged();

    void contentsMoved();

public slots:
    void createNew(int imageWidth, int imageHeight, bool transparentBackground);

    bool exportImage(const QUrl &url);
    void resize(int width, int height);
    void crop(const QRect &rect);
    void moveContents(int x, int y, bool onlyVisibleContents);

    void addNewLayer();
    void deleteCurrentLayer();
    void moveCurrentLayerUp();
    void moveCurrentLayerDown();
    void mergeCurrentLayerUp();
    void mergeCurrentLayerDown();
    void duplicateCurrentLayer();
    void setLayerName(int layerIndex, const QString &name);
    void setLayerVisible(int layerIndex, bool visible);
    void setLayerOpacity(int layerIndex, qreal opacity);

    void addAnimation();
    void duplicateAnimation(int index);
    /*
        Animation settings are modified directly to simplify the live preview,
        so that's why there are no arguments besides the name; the modifications
        are done and then this can be called to push a ModifyAnimationCommand
        which gathers the already-modified values itself.
    */
    void modifyAnimation(int index);
    void moveCurrentAnimationUp();
    void moveCurrentAnimationDown();
    void removeAnimation(int index);

protected:
    void doLoad(const QUrl &url) override;
    void doClose() override;
    bool doSaveAs(const QUrl &url) override;

private:
    friend class AddLayerCommand;
    friend class ChangeLayeredImageCanvasSizeCommand;
    friend class ChangeLayeredImageSizeCommand;
    friend class ChangeLayerOrderCommand;
    friend class ChangeLayerNameCommand;
    friend class ChangeLayerVisibleCommand;
    friend class ChangeLayerOpacityCommand;
    friend class DeleteLayerCommand;
    friend class MergeLayersCommand;
    friend class DuplicateLayerCommand;
    friend class MoveLayeredImageContentsCommand;

    bool isValidIndex(int index) const;

    void doSetCanvasSize(const QVector<QImage> &newImages);
    void doSetImageSize(const QVector<QImage> &newImages);

    void doMoveContents(const QVector<QImage> &newImages);

    void addNewLayer(int imageWidth, int imageHeight, bool transparent, bool undoable = true);
    void addLayerAboveAll(ImageLayer *imageLayer);
    void addLayer(ImageLayer *imageLayer, int index);
    void moveLayer(int fromIndex, int toIndex);
    void mergeLayers(int sourceIndex, int targetIndex);
    ImageLayer *takeLayer(int index);
    void setLayerImage(int index, const QImage &image);

    QString expandLayerNameVariables(const QString &layerFileNamePrefix) const;

    friend QDebug operator<<(QDebug debug, const LayeredImageProject *project);

    // Lowest index == layer with lowest Z order.
    QVector<ImageLayer*> mLayers;
    int mCurrentLayerIndex;
    // Give each layer a unique name based on the layers created so far.
    int mLayersCreated;
    bool mAutoExportEnabled;
    bool mUsingAnimation;
    bool mHasUsedAnimation;
    AnimationSystem mAnimationSystem;
};

#endif // LAYEREDIMAGEPROJECT_H
