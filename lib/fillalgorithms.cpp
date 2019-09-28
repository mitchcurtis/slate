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

#include "swatchcolour.h"
#include "texturedfillparameters.h"
#include "tile.h"
#include "tilesetproject.h"

Q_LOGGING_CATEGORY(lcPixelFloodFill, "app.pixelFloodFill")
Q_LOGGING_CATEGORY(lcTileFloodFill, "app.tileFloodFill")
Q_LOGGING_CATEGORY(lcSwatchTexturedFill, "app.swatchTexturedFill")

uint qHash(const QPoint &point, uint seed)
{
    return qHash(point.x() ^ seed) ^ point.y();
}

QColor FillColourProvider::colour(const QColor &baseColour) const
{
    return baseColour;
}

bool FillColourProvider::allowsNoOpFills() const
{
    return false;
}

bool FillColourProvider::canProvideColours() const
{
    return true;
}

QString FillColourProvider::debugName() const
{
    return "FillColourProvider";
}

struct ImagePixelHash {
    explicit ImagePixelHash(int hashSize, int imageWidth) : mImageWidth{imageWidth}, mData(hashSize, 0) {}

    int getImageIndex(const QPoint& point) {
        return (point.y() * mImageWidth) + point.x();
    }

    void insert(const QPoint& point) {
        mData[getImageIndex(point)] = 1;
    }

    bool contains(const QPoint& point) {
        return mData.at(getImageIndex(point)) == 1;
    }

    int size() {
        return static_cast<int>(std::count_if(mData.constBegin(), mData.constEnd(), [](int index){ return index == 1;}));
    }

    const int mImageWidth;
    QVector<int> mData;
};

QImage imagePixelFloodFill(const QImage *image, const QPoint &startPos, const QColor &targetColour,
    const QColor &replacementColour, const FillColourProvider &fillColourProvider)
{
    qCDebug(lcPixelFloodFill) << "attempting to fill starting with pixel at" << startPos << "...";

    QImage filledImage = *image;
    const QRect imageBounds(0, 0, filledImage.width(), filledImage.height());
    if (!imageBounds.contains(startPos)) {
        qCDebug(lcPixelFloodFill).nospace() << "The pixel at " << startPos
            << " is not within the image bounds (" << imageBounds << ")";
        return QImage();
    }

    if (!fillColourProvider.allowsNoOpFills() && filledImage.pixelColor(startPos) == replacementColour) {
        qCDebug(lcPixelFloodFill).nospace() << "The pixel at " << startPos
            << " (" << filledImage.pixelColor(startPos).name(QColor::HexArgb) << ") "
            << "is the same as what we want to replace it with (" << replacementColour.name(QColor::HexArgb) << ") "
            << "and the fill colour provider (" << fillColourProvider.debugName() << ") doesn't allow this";
        return QImage();
    }

    if (filledImage.pixelColor(startPos) != targetColour) {
        qCDebug(lcPixelFloodFill).nospace() << "The pixel at " << startPos
            << " (" << filledImage.pixelColor(startPos).name(QColor::HexArgb) << ") "
            << "is not the same as our target colour: " << targetColour.name(QColor::HexArgb);
        return QImage();
    }

    if (!fillColourProvider.canProvideColours()) {
        qCDebug(lcPixelFloodFill).nospace() << "The fill colour provider"
            << &fillColourProvider << "cannot provide colours for us";
        return QImage();
    }

    QQueue<QPoint> queue;
    queue.append(startPos);
    // TODO: use image to track filled positions instead
    ImagePixelHash filledPositions{filledImage.width() * filledImage.height(), filledImage.width()};
    filledPositions.insert(startPos);
    filledImage.setPixelColor(startPos, fillColourProvider.colour(replacementColour));

    for (int i = 0; i < queue.size(); ++i) {
        QPoint node = queue.at(i);
        QPoint west = node;
        QPoint east = node;

        while (true) {
            QPoint newWest = west - QPoint(1, 0);
            if (imageBounds.contains(newWest) && !filledPositions.contains(newWest) && image->pixelColor(newWest) == targetColour) {
                west = newWest;
            } else {
                break;
            }
        }

        while (true) {
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

    qCDebug(lcPixelFloodFill) << "... filled" << filledPositions.size() << "pixels.";
    return filledImage;
}

QImage imageGreedyPixelFill(const QImage *image, const QPoint &startPos, const QColor &targetColour,
    const QColor &replacementColour, const FillColourProvider &fillColourProvider)
{
    const QRect imageBounds(0, 0, image->width(), image->height());
    if (!imageBounds.contains(startPos)) {
        qCDebug(lcPixelFloodFill).nospace() << "The pixel at " << startPos
            << " is not within the image bounds (" << imageBounds << ")";
        return QImage();
    }

    if (image->pixelColor(startPos) == replacementColour) {
        qCDebug(lcPixelFloodFill).nospace() << "The pixel at " << startPos
            << " (" << image->pixelColor(startPos).name(QColor::HexArgb) << ") "
            << "is the same as what we want to replace it with: " << replacementColour.name(QColor::HexArgb);
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

class VarianceTextureFillColourProvider : public FillColourProvider
{
public:
    VarianceTextureFillColourProvider(const TexturedFillParameters &parameters) :
        mParameters(parameters)
    {
    }

    bool allowsNoOpFills() const override
    {
        return true;
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

    QString debugName() const override
    {
        return "VarianceTextureFillColourProvider";
    }

    const TexturedFillParameters &mParameters;
};

class SwatchTextureFillColourProvider : public FillColourProvider
{
public:
    SwatchTextureFillColourProvider(const TexturedFillParameters &parameters) :
        mParameters(parameters)
    {
        if (mParameters.swatch()->colours().isEmpty()) {
            qWarning() << "Textured fill swatch is empty!";
            return;
        }

        qCDebug(lcSwatchTexturedFill) << "calculating chances and probability sum";
        qCDebug(lcSwatchTexturedFill) << "swatch probabilities:" << mParameters.swatch()->probabilities();

        const auto probabilities = mParameters.swatch()->probabilities();
        const int probabilityCount = probabilities.size();
        mChances.resize(probabilityCount);

        // If there are ten colours, and each one has 100% probability,
        // then its "chance" is 0.10.
        // We calculate "chances" as a way of normalising the probabilities
        // so that we can easily check which one wins.
        const qreal oneHundredPercentProbability = 1.0 / probabilityCount;

        for (int i = 0; i < probabilityCount; ++i) {
            const qreal probability = probabilities.at(i);
            const qreal chance = probability * oneHundredPercentProbability;
            mChances[i] = chance;
            mChanceSum += chance;
        }

        // I'm not 100% sure, but I think this is necessary.
        // If the chances are [0.5, 0.1] and the random number is 0.05,
        // the first one might be chosen more often than it should.
        // The second number will only be chosen when the number is
        // greater than the first but less than both combined;
        // i.e. 0.5001, 0.5999, etc. So I guess technically
        // the second one can be chosen, but perhaps less often? I dunno.
        // It seems to work fine without sorting though.
        // TODO: colours also need to be sorted if we do this, otherwise it won't work correctly.
//        std::sort(mChances.begin(), mChances.end());

        if (qFuzzyIsNull(mChanceSum))
            qWarning() << "Sum of chances for textured fill swatch colours is zero!";

        qCDebug(lcSwatchTexturedFill) << "chances:" << mChances;
        qCDebug(lcSwatchTexturedFill) << "chance sum:" << mChanceSum;
    }

    bool allowsNoOpFills() const override
    {
        return true;
    }

    bool canProvideColours() const override
    {
        return mParameters.swatch()->hasNonZeroProbabilitySum();
    }

    QColor colour(const QColor &) const override
    {
        const auto randomGen = QRandomGenerator::global();
        const QVector<SwatchColour> swatchColours = mParameters.swatch()->colours();

        // We can't just use 1 for the maximum, because if there are two colours and
        // one has 0.3 chance and the other 0.5, then the latter will never
        // get chosen (1 - 0.3 = 0.7, which is greater than 0.5).
        qreal randomNumber = toRange(randomGen->generateDouble(), 0, mChanceSum);
        for (int i = 0; i < mChances.size(); ++i) {
            const qreal chance = mChances.at(i);
            if (randomNumber <= chance)
                return swatchColours.at(i).colour();
            randomNumber -= chance;
        }

        Q_UNREACHABLE();
    }

    QString debugName() const override
    {
        return "SwatchTextureFillColourProvider";
    }

    const TexturedFillParameters &mParameters;
    QVector<qreal> mChances;
    qreal mChanceSum = 0;
};

QImage texturedFill(const QImage *image, const QPoint &startPos, const QColor &targetColour,
    const QColor &replacementColour, const TexturedFillParameters &parameters)
{
    if (parameters.type() == TexturedFillParameters::VarianceFillType)
        return imagePixelFloodFill(image, startPos, targetColour, replacementColour, VarianceTextureFillColourProvider(parameters));

    // Swatch.
    return imagePixelFloodFill(image, startPos, targetColour, replacementColour, SwatchTextureFillColourProvider(parameters));
}

QImage greedyTexturedFill(const QImage *image, const QPoint &startPos, const QColor &targetColour,
    const QColor &replacementColour, const TexturedFillParameters &parameters)
{
    if (parameters.type() == TexturedFillParameters::VarianceFillType)
        return imageGreedyPixelFill(image, startPos, targetColour, replacementColour, VarianceTextureFillColourProvider(parameters));

    // Swatch.
    return imageGreedyPixelFill(image, startPos, targetColour, replacementColour, SwatchTextureFillColourProvider(parameters));
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
