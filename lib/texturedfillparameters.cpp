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

#include "texturedfillparameters.h"

TexturedFillVarianceParameter::TexturedFillVarianceParameter()
{
}

bool TexturedFillVarianceParameter::isEnabled() const
{
    return mEnabled;
}

void TexturedFillVarianceParameter::setEnabled(bool enabled)
{
    if (enabled == mEnabled)
        return;

    mEnabled = enabled;
    emit enabledChanged();
}

qreal TexturedFillVarianceParameter::varianceLowerBound() const
{
    return mVarianceLowerBound;
}

void TexturedFillVarianceParameter::setVarianceLowerBound(const qreal &varianceLowerBound)
{
    if (varianceLowerBound == mVarianceUpperBound)
        return;

    mVarianceLowerBound = varianceLowerBound;
    emit varianceLowerBoundChanged();
}

qreal TexturedFillVarianceParameter::varianceUpperBound() const
{
    return mVarianceUpperBound;
}

void TexturedFillVarianceParameter::setVarianceUpperBound(const qreal &varianceUpperBound)
{
    if (varianceUpperBound == mVarianceUpperBound)
        return;

    mVarianceUpperBound = varianceUpperBound;
    emit varianceUpperBoundChanged();
}

void TexturedFillVarianceParameter::copy(const TexturedFillVarianceParameter &other)
{
    setEnabled(other.isEnabled());
    setVarianceLowerBound(other.varianceLowerBound());
    setVarianceUpperBound(other.varianceUpperBound());
}

void TexturedFillVarianceParameter::reset()
{
    mEnabled = false;
    mVarianceLowerBound = 0;
    mVarianceUpperBound = 0;
}

TexturedFillParameters::TexturedFillParameters(QObject *parent) :
    QObject(parent)
{
    mSwatch.setObjectName("texturedFillParametersSwatch");
}

TexturedFillParameters::TexturedFillType TexturedFillParameters::type() const
{
    return mType;
}

void TexturedFillParameters::setType(TexturedFillParameters::TexturedFillType type)
{
    if (type == mType)
        return;

    mType = type;
    emit typeChanged();
}

TexturedFillVarianceParameter *TexturedFillParameters::hue()
{
    return &mHue;
}

const TexturedFillVarianceParameter *TexturedFillParameters::hue() const
{
    return &mHue;
}

TexturedFillVarianceParameter *TexturedFillParameters::saturation()
{
    return &mSaturation;
}

const TexturedFillVarianceParameter *TexturedFillParameters::saturation() const
{
    return &mSaturation;
}

TexturedFillVarianceParameter *TexturedFillParameters::lightness()
{
    return &mLightness;
}

const TexturedFillVarianceParameter *TexturedFillParameters::lightness() const
{
    return &mLightness;
}

ProbabilitySwatch *TexturedFillParameters::swatch()
{
    return &mSwatch;
}

const ProbabilitySwatch *TexturedFillParameters::swatch() const
{
    return &mSwatch;
}

void TexturedFillParameters::copy(TexturedFillParameters *other)
{
    setType(other->type());

    mHue.copy(*other->hue());
    mSaturation.copy(*other->saturation());
    mLightness.copy(*other->lightness());

    mSwatch.copy(*other->swatch());
}

void TexturedFillParameters::reset()
{
    mType = VarianceFillType;

    mHue.reset();
    mSaturation.reset();
    mLightness.reset();

    mSwatch.reset();
}
