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

#include "newprojectvalidator.h"

#include <QImage>
#include <QtMath>

NewProjectValidator::NewProjectValidator(QObject *parent) :
    FileValidator(parent),
    mCalculatedTilesWide(0),
    mCalculatedTilesHigh(0),
    mTileWidth(0),
    mTileHeight(0)
{
    setTreatAsImage(true);
}

int NewProjectValidator::calculatedTilesWide() const
{
    return mCalculatedTilesWide;
}

void NewProjectValidator::setCalculatedTilesWide(int calculatedTilesWide)
{
    if (calculatedTilesWide == mCalculatedTilesWide)
        return;

    mCalculatedTilesWide = calculatedTilesWide;
    emit calculatedTilesWideChanged();
}

int NewProjectValidator::calculatedTilesHigh() const
{
    return mCalculatedTilesHigh;
}

void NewProjectValidator::setCalculatedTilesHigh(int calculatedTilesHigh)
{
    if (calculatedTilesHigh == mCalculatedTilesHigh)
        return;

    mCalculatedTilesHigh = calculatedTilesHigh;
    emit calculatedTilesHighChanged();
}

void NewProjectValidator::setTileWidthErrorMessage(const QString &errorMessage)
{
    if (errorMessage == mTileWidthErrorMessage)
        return;

    const bool wasValid = isTileWidthValid();

    mTileWidthErrorMessage = errorMessage;
    emit tileWidthErrorMessageChanged();

    if (isTileWidthValid() != wasValid)
        emit tileWidthValidChanged();
}

void NewProjectValidator::setTileHeightErrorMessage(const QString &errorMessage)
{
    if (errorMessage == mTileHeightErrorMessage)
        return;

    const bool wasValid = isTileHeightValid();

    mTileHeightErrorMessage = errorMessage;
    emit tileHeightErrorMessageChanged();

    if (isTileHeightValid() != wasValid)
        emit tileHeightValidChanged();
}

int NewProjectValidator::tileWidth() const
{
    return mTileWidth;
}

void NewProjectValidator::setTileWidth(int width)
{
    if (width == mTileWidth)
        return;

    mTileWidth = width;
    validate();
    emit tileWidthChanged();
}

int NewProjectValidator::tileHeight() const
{
    return mTileHeight;
}

void NewProjectValidator::setTileHeight(int height)
{
    if (height == mTileHeight)
        return;

    mTileHeight = height;
    validate();
    emit tileHeightChanged();
}

QString NewProjectValidator::tileWidthErrorMessage() const
{
    return mTileWidthErrorMessage;
}

QString NewProjectValidator::tileHeightErrorMessage() const
{
    return mTileHeightErrorMessage;
}

bool NewProjectValidator::isTileWidthValid() const
{
    return mTileWidthErrorMessage.isEmpty();
}

bool NewProjectValidator::isTileHeightValid() const
{
    return mTileHeightErrorMessage.isEmpty();
}

void NewProjectValidator::validate()
{
    // This is called by the tile width/height setters, so we can't
    // always be guaranteed that FileValidator has done file validation.
    if (mUrl.isEmpty())
        return;

    const QImage tilesetImage(mUrl.toLocalFile());

    QString widthError;
    const qreal tilesWide = tilesetImage.width() / qreal(mTileWidth);
    if (tilesWide < 1.0) {
        widthError = QString::fromLatin1("Tile width (%1) is too large for the image's width (%2)")
            .arg(mTileWidth).arg(tilesetImage.width());
    }

    if (widthError.isEmpty()) {
        const qreal widthDifference = tilesWide - qFloor(tilesWide);
        if (widthDifference != 0) {
            widthError = QString::fromLatin1("Tile width (%1) is not a multiple of the image's width (%2)")
                .arg(mTileWidth).arg(tilesetImage.width());
        }
    }

    setTileWidthErrorMessage(widthError);

    QString heightError;
    const qreal tilesHigh = tilesetImage.height() / qreal(mTileHeight);
    if (tilesHigh < 1.0) {
        heightError = QString::fromLatin1("Tile height (%1) is too large for the image's height (%2)")
            .arg(mTileHeight).arg(tilesetImage.height());
    }

    if (heightError.isEmpty()) {
        const qreal heightDifference = tilesHigh - qFloor(tilesHigh);
        if (heightDifference != 0) {
            heightError = QString::fromLatin1("Tile height (%1) is not a multiple of the image's height (%2)")
                .arg(mTileHeight).arg(tilesetImage.height());
        }
    }

    setTileHeightErrorMessage(heightError);

    if (isTileWidthValid() && isTileHeightValid()) {
        setCalculatedTilesWide(tilesWide);
        setCalculatedTilesHigh(tilesHigh);
    }
}
