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

#include "fillalgorithms.h"

#include <QDebug>
#include <QColor>
#include <QImage>
#include <QLoggingCategory>
#include <QRandomGenerator>
#include <QQueue>

#include "texturedfillparameters.h"
#include "tile.h"
#include "tilesetproject.h"

Q_LOGGING_CATEGORY(lcPixelFloodFill, "app.pixelFloodFill")
Q_LOGGING_CATEGORY(lcTileFloodFill, "app.tileFloodFill")

uint qHash(const QPoint &point, uint seed)
{
    return qHash(point.x() ^ seed) ^ point.y();
}

QColor FillColourProvider::colour(const QColor &baseColour) const
{
    return baseColour;
}

// https://en.wikipedia.org/wiki/Flood_fill#Alternative_implementations
// "[...] use a loop for the west and east directions as an optimization to avoid the overhead of stack or queue management."
QVector<QPoint> imagePixelFloodFill(const QImage *image, const QPoint &startPos, const QColor &targetColour,
    const QColor &replacementColour)
{
    QSet<QPoint> filledPositions;
    const QRect imageBounds(0, 0, image->width(), image->height());
    if (!imageBounds.contains(startPos)) {
        return QVector<QPoint>();
    }

    if (image->pixelColor(startPos) == replacementColour) {
        // The pixel at startPos is already the colour that we want to replace it with.
        return QVector<QPoint>();
    }

    if (image->pixelColor(startPos) != targetColour) {
        return QVector<QPoint>();
    }

    QQueue<QPoint> queue;
    queue.append(startPos);
    filledPositions.insert(startPos);

    for (int i = 0; i < queue.size(); ++i) {
        QPoint node = queue.at(i);
        QPoint west = node;
        QPoint east = node;

        while (1) {
            QPoint newWest = west - QPoint(1, 0);
            if (imageBounds.contains(newWest) && !filledPositions.contains(newWest) && image->pixelColor(newWest) == targetColour) {
                west = newWest;
            } else {
                break;
            }
        }

        while (1) {
            QPoint newEast = east + QPoint(1, 0);
            if (imageBounds.contains(newEast) && !filledPositions.contains(newEast) && image->pixelColor(newEast) == targetColour) {
                east = newEast;
            } else {
                break;
            }
        }

        for (int x = west.x(); x <= east.x(); ++x) {
            const QPoint n = QPoint(x, node.y());
            // This avoids startPos being added twice.
            if (!filledPositions.contains(n)) {
                queue.append(n);
                filledPositions.insert(n);
            }

            const QPoint north(n - QPoint(0, 1));
            if (imageBounds.contains(north) && !filledPositions.contains(north) && image->pixelColor(north) == targetColour) {
                queue.append(north);
                filledPositions.insert(north);
            }

            const QPoint south(n + QPoint(0, 1));
            if (imageBounds.contains(south) && !filledPositions.contains(south) && image->pixelColor(south) == targetColour) {
                queue.append(south);
                filledPositions.insert(south);
            }
        }
    }

    // TODO: convert from QSet => QVector manually, as this is just silly
    return filledPositions.toList().toVector();
}

QImage imagePixelFloodFill2(const QImage *image, const QPoint &startPos, const QColor &targetColour,
    const QColor &replacementColour, const FillColourProvider &fillColourProvider)
{
    QImage filledImage = *image;
    const QRect imageBounds(0, 0, filledImage.width(), filledImage.height());
    if (!imageBounds.contains(startPos)) {
        return QImage();
    }

    if (filledImage.pixelColor(startPos) == replacementColour) {
        // The pixel at startPos is already the colour that we want to replace it with.
        return QImage();
    }

    if (filledImage.pixelColor(startPos) != targetColour)
        return QImage();

    QQueue<QPoint> queue;
    queue.append(startPos);
    // TODO: use image to track filled positions instead
    QSet<QPoint> filledPositions;
    filledPositions.insert(startPos);
    filledImage.setPixelColor(startPos, fillColourProvider.colour(replacementColour));

    for (int i = 0; i < queue.size(); ++i) {
        QPoint node = queue.at(i);
        QPoint west = node;
        QPoint east = node;

        while (1) {
            QPoint newWest = west - QPoint(1, 0);
            if (imageBounds.contains(newWest) && !filledPositions.contains(newWest) && image->pixelColor(newWest) == targetColour) {
                west = newWest;
            } else {
                break;
            }
        }

        while (1) {
            QPoint newEast = east + QPoint(1, 0);
            if (imageBounds.contains(newEast) && !filledPositions.contains(newEast) && image->pixelColor(newEast) == targetColour) {
                east = newEast;
            } else {
                break;
            }
        }

        for (int x = west.x(); x <= east.x(); ++x) {
            const QPoint n = QPoint(x, node.y());
            // This avoids startPos being added twice.
            if (!filledPositions.contains(n)) {
                queue.append(n);
                filledPositions.insert(n);
                filledImage.setPixelColor(n, fillColourProvider.colour(replacementColour));
            }

            const QPoint north(n - QPoint(0, 1));
            if (imageBounds.contains(north) && !filledPositions.contains(north) && image->pixelColor(north) == targetColour) {
                queue.append(north);
                filledPositions.insert(north);
                filledImage.setPixelColor(north, fillColourProvider.colour(replacementColour));
            }

            const QPoint south(n + QPoint(0, 1));
            if (imageBounds.contains(south) && !filledPositions.contains(south) && image->pixelColor(south) == targetColour) {
                queue.append(south);
                filledPositions.insert(south);
                filledImage.setPixelColor(south, fillColourProvider.colour(replacementColour));
            }
        }
    }

    return filledImage;
}

QVector<QPoint> imageGreedyPixelFill(const QImage *image, const QPoint &startPos, const QColor &targetColour, const QColor &replacementColour)
{
    QVector<QPoint> filledPositions;
    const QRect imageBounds(0, 0, image->width(), image->height());
    if (!imageBounds.contains(startPos)) {
        return filledPositions;
    }

    if (image->pixelColor(startPos) == replacementColour) {
        // The pixel at startPos is already the colour that we want to replace it with.
        return filledPositions;
    }

    for (int y = 0; y < image->height(); ++y) {
        for (int x = 0; x < image->width(); ++x) {
            if (image->pixelColor(x, y) == targetColour) {
                filledPositions.append(QPoint(x, y));
            }
        }
    }

    return filledPositions;
}

QImage imageGreedyPixelFill2(const QImage *image, const QPoint &startPos, const QColor &targetColour,
    const QColor &replacementColour, const FillColourProvider &fillColourProvider)
{
    const QRect imageBounds(0, 0, image->width(), image->height());
    if (!imageBounds.contains(startPos))
        return QImage();

    if (image->pixelColor(startPos) == replacementColour) {
        // The pixel at startPos is already the colour that we want to replace it with.
        return QImage();
    }

    QImage filledImage(*image);
    for (int y = 0; y < image->height(); ++y) {
        for (int x = 0; x < image->width(); ++x) {
            if (image->pixelColor(x, y) == targetColour) {
                filledImage.setPixelColor(x, y, fillColourProvider.colour(replacementColour));
            }
        }
    }
    return filledImage;
}

qreal toRange(qreal randomNumber, qreal min, qreal max)
{
    return randomNumber * (max - min) + min;
}

class TextureFillColourProvider : public FillColourProvider
{
public:
    TextureFillColourProvider(const TexturedFillParameters &parameters) :
        mParameters(parameters)
    {
    }

    QColor colour(const QColor &baseColour) const override
    {
        const auto randomGen = QRandomGenerator::global();

        const QColor baseColourAsHsl = baseColour.toHsl();

        qreal hue = baseColourAsHsl.hslHueF();
        if (mParameters.hue()->isEnabled()) {
            const qreal variance = toRange(randomGen->generateDouble(),
                mParameters.hue()->varianceLowerBound(), mParameters.hue()->varianceUpperBound());
            hue = qBound(0.0, hue + variance, 1.0);
        }

        qreal saturation = baseColourAsHsl.hslSaturationF();
        if (mParameters.saturation()->isEnabled()) {
            const qreal variance = toRange(randomGen->generateDouble(),
                mParameters.saturation()->varianceLowerBound(), mParameters.saturation()->varianceUpperBound());
            saturation = qBound(0.0, saturation + variance, 1.0);
        }

        qreal lightness = baseColourAsHsl.lightnessF();
        if (mParameters.lightness()->isEnabled()) {
            const qreal variance = toRange(randomGen->generateDouble(),
                mParameters.lightness()->varianceLowerBound(), mParameters.lightness()->varianceUpperBound());
            lightness = qBound(0.0, lightness + variance, 1.0);
        }

        const QColor newColour(QColor::fromHslF(hue, saturation, lightness));
        return newColour;
    }

    const TexturedFillParameters &mParameters;
};

QImage texturedFill(const QImage *image, const QPoint &startPos, const QColor &targetColour,
    const QColor &replacementColour, const TexturedFillParameters &parameters)
{
    return imagePixelFloodFill2(image, startPos, targetColour, replacementColour, TextureFillColourProvider(parameters));
}

QImage greedyTexturedFill(const QImage *image, const QPoint &startPos, const QColor &targetColour,
    const QColor &replacementColour, const TexturedFillParameters &parameters)
{
    return imageGreedyPixelFill2(image, startPos, targetColour, replacementColour, TextureFillColourProvider(parameters));
}

// TODO: convert these to non-recursive algorithms as above
void tilesetPixelFloodFill(const Tile *tile, const QPoint &pos, const QColor &targetColour,
    const QColor &replacementColour, QVector<QPoint> &filledPositions)
{
    qCDebug(lcPixelFloodFill) << "attempting to fill pixel at" << pos << "...";

    if (filledPositions.contains(pos)) {
        // This tile has already been filled (would this ever happen?)
        qCDebug(lcPixelFloodFill) << "already filled";
        return;
    }

    if (tile->pixelColor(pos) == replacementColour) {
        qCDebug(lcPixelFloodFill) << "hit the same colour as replacement colour; returning";
        return;
    }

    if (tile->pixelColor(pos) != targetColour) {
        qCDebug(lcPixelFloodFill) << "hit a different colour; returning";
        return;
    }

    qCDebug(lcPixelFloodFill) << "filling!";
    filledPositions.append(pos);

    const QRect tileBounds(0, 0, tile->sourceRect().width(), tile->sourceRect().height());
    const QPoint north = pos - QPoint(0, 1);
    const QPoint south = pos + QPoint(0, 1);
    const QPoint east = pos + QPoint(1, 0);
    const QPoint west = pos - QPoint(1, 0);

    if (tileBounds.contains(north)) {
        tilesetPixelFloodFill(tile, north, targetColour, replacementColour, filledPositions);
    } else {
        qCDebug(lcPixelFloodFill) << north << "is out of bounds" << "( tileBounds =" << tileBounds << ")";
    }
    if (tileBounds.contains(south)) {
        tilesetPixelFloodFill(tile, south, targetColour, replacementColour, filledPositions);
    } else {
        qCDebug(lcPixelFloodFill) << south << "is out of bounds" << "( tileBounds =" << tileBounds << ")";
    }
    if (tileBounds.contains(east)) {
        tilesetPixelFloodFill(tile, east, targetColour, replacementColour, filledPositions);
    } else {
        qCDebug(lcPixelFloodFill) << east << "is out of bounds" << "( tileBounds =" << tileBounds << ")";
    }
    if (tileBounds.contains(west)) {
        tilesetPixelFloodFill(tile, west, targetColour, replacementColour, filledPositions);
    } else {
        qCDebug(lcPixelFloodFill) << west << "is out of bounds" << "( tileBounds =" << tileBounds << ")";
    }
}

void tilesetTileFloodFill(const TilesetProject *project, const Tile *tile, const QPoint &tilePos,
    int targetTile, int replacementTile, QVector<QPoint> &filledTilePositions)
{
    qCDebug(lcTileFloodFill) << "attempting to fill pixel at" << tilePos << "...";

    if (filledTilePositions.contains(tilePos)) {
        // This tile has already been filled (would this ever happen?)
        qCDebug(lcTileFloodFill) << "already filled";
        return;
    }

    const int tileIdAtTilePos = project->tileIdAtTilePos(tilePos);
    if (tileIdAtTilePos == replacementTile) {
        qCDebug(lcTileFloodFill) << "hit the same tile as replacement tile; returning";
        return;
    }

    if (tileIdAtTilePos != targetTile) {
        qCDebug(lcTileFloodFill) << "hit a different colour; returning";
        return;
    }

    qCDebug(lcTileFloodFill) << "filling!";
    filledTilePositions.append(tilePos);

    const QPoint north = tilePos - QPoint(0, 1);
    const QPoint south = tilePos + QPoint(0, 1);
    const QPoint east = tilePos + QPoint(1, 0);
    const QPoint west = tilePos - QPoint(1, 0);

    if (project->isTilePosWithinBounds(north)) {
        tilesetTileFloodFill(project, tile, north, targetTile, replacementTile, filledTilePositions);
    } else {
        qCDebug(lcTileFloodFill) << north << "is out of bounds";
    }
    if (project->isTilePosWithinBounds(south)) {
        tilesetTileFloodFill(project, tile, south, targetTile, replacementTile, filledTilePositions);
    } else {
        qCDebug(lcTileFloodFill) << south << "is out of bounds";
    }
    if (project->isTilePosWithinBounds(east)) {
        tilesetTileFloodFill(project, tile, east, targetTile, replacementTile, filledTilePositions);
    } else {
        qCDebug(lcTileFloodFill) << east << "is out of bounds";
    }
    if (project->isTilePosWithinBounds(west)) {
        tilesetTileFloodFill(project, tile, west, targetTile, replacementTile, filledTilePositions);
    } else {
        qCDebug(lcTileFloodFill) << west << "is out of bounds";
    }
}
