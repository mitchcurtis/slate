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

#ifndef TEXTUREDFILLPARAMETERS_H
#define TEXTUREDFILLPARAMETERS_H

#include <QObject>
#include <QQmlEngine>

#include "slate-global.h"
#include "probabilityswatch.h"

class SLATE_EXPORT TexturedFillVarianceParameter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(qreal varianceLowerBound READ varianceLowerBound WRITE setVarianceLowerBound NOTIFY varianceLowerBoundChanged)
    Q_PROPERTY(qreal varianceUpperBound READ varianceUpperBound WRITE setVarianceUpperBound NOTIFY varianceUpperBoundChanged)
    QML_ELEMENT
    QML_UNCREATABLE("")

public:
    TexturedFillVarianceParameter();

    bool isEnabled() const;
    void setEnabled(bool isEnabled);

    qreal varianceLowerBound() const;
    void setVarianceLowerBound(const qreal &varianceLowerBound);

    qreal varianceUpperBound() const;
    void setVarianceUpperBound(const qreal &varianceUpperBound);

    void copy(const TexturedFillVarianceParameter &other);

    void reset();

signals:
    void enabledChanged();
    void varianceLowerBoundChanged();
    void varianceUpperBoundChanged();

private:
    bool mEnabled = false;
    qreal mVarianceLowerBound = 0;
    qreal mVarianceUpperBound = 0;
};

class SLATE_EXPORT TexturedFillParameters : public QObject
{
    Q_OBJECT
    Q_PROPERTY(TexturedFillType type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(TexturedFillVarianceParameter *hue READ hue CONSTANT FINAL)
    Q_PROPERTY(TexturedFillVarianceParameter *saturation READ saturation CONSTANT FINAL)
    Q_PROPERTY(TexturedFillVarianceParameter *lightness READ lightness CONSTANT FINAL)
    Q_PROPERTY(ProbabilitySwatch *swatch READ swatch CONSTANT FINAL)
    QML_ELEMENT
    QML_UNCREATABLE("")

public:
    enum TexturedFillType {
        VarianceFillType,
        SwatchFillType
    };
    Q_ENUM(TexturedFillType)

    explicit TexturedFillParameters(QObject *parent = nullptr);

    TexturedFillType type() const;
    void setType(TexturedFillType type);

    TexturedFillVarianceParameter *hue();
    const TexturedFillVarianceParameter *hue() const;

    TexturedFillVarianceParameter *saturation();
    const TexturedFillVarianceParameter *saturation() const;

    TexturedFillVarianceParameter *lightness();
    const TexturedFillVarianceParameter *lightness() const;

    ProbabilitySwatch *swatch();
    const ProbabilitySwatch *swatch() const;

    Q_INVOKABLE void copy(TexturedFillParameters *other);

    void reset();

signals:
    void typeChanged();

private:
    TexturedFillType mType = VarianceFillType;

    TexturedFillVarianceParameter mHue;
    TexturedFillVarianceParameter mSaturation;
    TexturedFillVarianceParameter mLightness;

    ProbabilitySwatch mSwatch;
};

#endif // TEXTUREDFILLPARAMETERS_H
