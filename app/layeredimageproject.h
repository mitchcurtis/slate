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

#ifndef LAYEREDIMAGEPROJECT_H
#define LAYEREDIMAGEPROJECT_H

#include <QDebug>
#include <QImage>

#include "project.h"

class ImageLayer;

class LayeredImageProject : public Project
{
    Q_OBJECT
    Q_PROPERTY(int currentLayerIndex READ currentLayerIndex WRITE setCurrentLayerIndex NOTIFY currentLayerIndexChanged)
    Q_PROPERTY(ImageLayer *currentLayer READ currentLayer NOTIFY postCurrentLayerChanged)
    Q_PROPERTY(int layerCount READ layerCount NOTIFY layerCountChanged)

public:
    LayeredImageProject();
    ~LayeredImageProject();

    ImageLayer *currentLayer();
    int currentLayerIndex() const;
    void setCurrentLayerIndex(int index);
    ImageLayer *layerAt(int index);
    const ImageLayer *layerAt(int index) const;
    int layerCount() const;

    Type type() const override;
    QSize size() const override;
    void setSize(const QSize &newSize) override;
    int widthInPixels() const override;
    int heightInPixels() const override;

    QImage flattenedImage(std::function<QImage(int)> layerSubstituteFunction = nullptr) const;

signals:
    void currentLayerIndexChanged();
    void preCurrentLayerChanged();
    void postCurrentLayerChanged();
    void layerCountChanged();

    void preLayerAdded(int index);
    void postLayerAdded(int index);
    void preLayerRemoved(int index);
    void postLayerRemoved(int index);
    void preLayerMoved(int fromIndex, int toIndex);
    void postLayerMoved(int fromIndex, int toIndex);

public slots:
    void createNew(int imageWidth, int imageHeight, bool transparentBackground);

    void load(const QUrl &url) override;
    void close() override;
    void saveAs(const QUrl &url) override;
    void exportImage(const QUrl &url);

    void addNewLayer();
    void deleteCurrentLayer();
    void moveCurrentLayerUp();
    void moveCurrentLayerDown();
    void setLayerName(int layerIndex, const QString &name);
    void setLayerVisible(int layerIndex, bool visible);
    void setLayerOpacity(int layerIndex, qreal opacity);

private:
    friend class AddLayerCommand;
    friend class ChangeLayeredImageCanvasSizeCommand;
    friend class ChangeLayerOrderCommand;
    friend class ChangeLayerNameCommand;
    friend class ChangeLayerVisibleCommand;
    friend class ChangeLayerOpacityCommand;
    friend class DeleteLayerCommand;

    bool isValidIndex(int index) const;

    void changeSize(const QSize &size);
    void addNewLayer(int imageWidth, int imageHeight, bool transparent, bool undoable = true);
    void addLayerAboveAll(ImageLayer *imageLayer);
    void addLayer(ImageLayer *imageLayer, int index);
    void moveLayer(int fromIndex, int toIndex);
    ImageLayer *takeLayer(int index);

    friend QDebug operator<<(QDebug debug, const LayeredImageProject *project);

    // Lowest index == layer with lowest Z order.
    QVector<ImageLayer*> mLayers;
    int mCurrentLayerIndex;
    // Give each layer a unique name based on the layers created so far.
    int mLayersCreated;
};

#endif // LAYEREDIMAGEPROJECT_H
