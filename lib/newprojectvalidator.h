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

#ifndef NEWPROJECTVALIDATOR_H
#define NEWPROJECTVALIDATOR_H

#include <QObject>
#include <QQmlEngine>
#include <QString>

#include "filevalidator.h"
#include "slate-global.h"

class SLATE_EXPORT NewProjectValidator : public FileValidator
{
    Q_OBJECT
    Q_PROPERTY(int calculatedTilesWide READ calculatedTilesWide NOTIFY calculatedTilesWideChanged)
    Q_PROPERTY(int calculatedTilesHigh READ calculatedTilesHigh NOTIFY calculatedTilesHighChanged)
    Q_PROPERTY(int tileWidth READ tileWidth WRITE setTileWidth NOTIFY tileWidthChanged)
    Q_PROPERTY(int tileHeight READ tileHeight WRITE setTileHeight NOTIFY tileHeightChanged)
    Q_PROPERTY(QString tileWidthErrorMessage READ tileWidthErrorMessage NOTIFY tileWidthErrorMessageChanged)
    Q_PROPERTY(QString tileHeightErrorMessage READ tileHeightErrorMessage NOTIFY tileHeightErrorMessageChanged)
    Q_PROPERTY(bool tileWidthValid READ isTileWidthValid NOTIFY tileWidthValidChanged)
    Q_PROPERTY(bool tileHeightValid READ isTileHeightValid NOTIFY tileHeightValidChanged)
    QML_ELEMENT

public:
    explicit NewProjectValidator(QObject *parent = nullptr);

    int calculatedTilesWide() const;
    int calculatedTilesHigh() const;
    int tileWidth() const;
    void setTileWidth(int tileWidth);
    int tileHeight() const;
    void setTileHeight(int tileHeight);

    QString tileWidthErrorMessage() const;
    QString tileHeightErrorMessage() const;
    bool isTileWidthValid() const;
    bool isTileHeightValid() const;

signals:
    void calculatedTilesWideChanged();
    void calculatedTilesHighChanged();
    void tileWidthChanged();
    void tileHeightChanged();
    void tileWidthErrorMessageChanged();
    void tileHeightErrorMessageChanged();
    void tileWidthValidChanged();
    void tileHeightValidChanged();

protected:
    void urlChange();

private:
    void setCalculatedTilesWide(int calculatedTilesWide);
    void setCalculatedTilesHigh(int calculatedTilesHigh);
    void setTileWidthErrorMessage(const QString &errorMessage);
    void setTileHeightErrorMessage(const QString &errorMessage);

    void validate() override;

    int mCalculatedTilesWide;
    int mCalculatedTilesHigh;
    int mTileWidth;
    int mTileHeight;
    QString mTileWidthErrorMessage;
    QString mTileHeightErrorMessage;
};

#endif // NEWPROJECTVALIDATOR_H
