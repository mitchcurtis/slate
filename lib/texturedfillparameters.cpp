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

TexturedFillParameter::TexturedFillParameter()
{
    reset();
}

bool TexturedFillParameter::isEnabled() const
{
    return mEnabled;
}

void TexturedFillParameter::setEnabled(bool enabled)
{
    if (enabled == mEnabled)
        return;

    mEnabled = enabled;
    emit enabledChanged();
}

qreal TexturedFillParameter::varianceLowerBound() const
{
    return mVarianceLowerBound;
}

void TexturedFillParameter::setVarianceLowerBound(const qreal &varianceLowerBound)
{
    if (varianceLowerBound == mVarianceUpperBound)
        return;

    mVarianceLowerBound = varianceLowerBound;
    emit varianceLowerBoundChanged();
}

qreal TexturedFillParameter::varianceUpperBound() const
{
    return mVarianceUpperBound;
}

void TexturedFillParameter::setVarianceUpperBound(const qreal &varianceUpperBound)
{
    if (varianceUpperBound == mVarianceUpperBound)
        return;

    mVarianceUpperBound = varianceUpperBound;
    emit varianceUpperBoundChanged();
}

void TexturedFillParameter::copy(const TexturedFillParameter &other)
{
    setEnabled(other.isEnabled());
    setVarianceLowerBound(other.varianceLowerBound());
    setVarianceUpperBound(other.varianceUpperBound());
}

void TexturedFillParameter::reset()
{
    mEnabled = false;
    mVarianceLowerBound = 0;
    mVarianceUpperBound = 0;
}

TexturedFillParameters::TexturedFillParameters(QObject *parent) :
    QObject(parent)
{
}

TexturedFillParameter *TexturedFillParameters::hue()
{
    return &mHue;
}

const TexturedFillParameter *TexturedFillParameters::hue() const
{
    return &mHue;
}

TexturedFillParameter *TexturedFillParameters::saturation()
{
    return &mSaturation;
}

const TexturedFillParameter *TexturedFillParameters::saturation() const
{
    return &mSaturation;
}

TexturedFillParameter *TexturedFillParameters::lightness()
{
    return &mLightness;
}

const TexturedFillParameter *TexturedFillParameters::lightness() const
{
    return &mLightness;
}

void TexturedFillParameters::copy(TexturedFillParameters *other)
{
    mHue.copy(*other->hue());
    mSaturation.copy(*other->saturation());
    mLightness.copy(*other->lightness());
}

void TexturedFillParameters::reset()
{
    mHue.reset();
    mSaturation.reset();
    mLightness.reset();
}
