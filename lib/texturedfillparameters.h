/*
    Copyright 2018, Mitch Curtis

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

#ifndef TEXTUREDFILLPARAMETERS_H
#define TEXTUREDFILLPARAMETERS_H

#include <QObject>

#include "slate-global.h"

class SLATE_EXPORT TexturedFillParameter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(qreal varianceLowerBound READ varianceLowerBound WRITE setVarianceLowerBound NOTIFY varianceLowerBoundChanged)
    Q_PROPERTY(qreal varianceUpperBound READ varianceUpperBound WRITE setVarianceUpperBound NOTIFY varianceUpperBoundChanged)

public:
    TexturedFillParameter();

    bool isEnabled() const;
    void setEnabled(bool isEnabled);

    qreal varianceLowerBound() const;
    void setVarianceLowerBound(const qreal &varianceLowerBound);

    qreal varianceUpperBound() const;
    void setVarianceUpperBound(const qreal &varianceUpperBound);

    void copy(const TexturedFillParameter &other);

    void reset();

signals:
    void enabledChanged();
    void varianceLowerBoundChanged();
    void varianceUpperBoundChanged();

private:
    bool mEnabled;
    qreal mVarianceLowerBound;
    qreal mVarianceUpperBound;
};

class SLATE_EXPORT TexturedFillParameters : public QObject
{
    Q_OBJECT
    Q_PROPERTY(TexturedFillParameter *hue READ hue CONSTANT FINAL)
    Q_PROPERTY(TexturedFillParameter *saturation READ saturation CONSTANT FINAL)
    Q_PROPERTY(TexturedFillParameter *lightness READ lightness CONSTANT FINAL)

public:
    explicit TexturedFillParameters(QObject *parent = nullptr);

    TexturedFillParameter *hue();
    const TexturedFillParameter *hue() const;

    TexturedFillParameter *saturation();
    const TexturedFillParameter *saturation() const;

    TexturedFillParameter *lightness();
    const TexturedFillParameter *lightness() const;

    Q_INVOKABLE void copy(TexturedFillParameters *other);

    void reset();

private:
    TexturedFillParameter mHue;
    TexturedFillParameter mSaturation;
    TexturedFillParameter mLightness;
};

#endif // TEXTUREDFILLPARAMETERS_H
