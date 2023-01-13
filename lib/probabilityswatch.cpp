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

#include "probabilityswatch.h"

#include <QJsonArray>
#include <QJsonObject>

ProbabilitySwatch::ProbabilitySwatch(QObject *parent) :
    Swatch(parent)
{
}

void ProbabilitySwatch::setProbability(int index, qreal probability)
{
    if (!isValidIndex(index))
        return;

    qreal &ourProbability = mProbabilities[index];
    const qreal oldProbability = ourProbability;
    if (qFuzzyCompare(probability, ourProbability))
        return;

    ourProbability = probability;
    calculateProbabilitySum();
    if (hasNonZeroProbabilitySum()) {
        // Changing this colour's probability didn't result in a zero probability sum, so it's OK.
        emit probabilityChanged(index);
    } else {
        // Changing this colour's probability resulted in a zero probability sum; revert the change.
        // This enforces the non-zero sum requirement in the UI, by preventing the slider from
        // going below the required value.
        ourProbability = oldProbability;
    }
}

void ProbabilitySwatch::addColoursWithProbabilities(const QVector<QColor> &colours, const QVector<qreal> &probabilities)
{
    if (colours.size() != probabilities.size()) {
        qWarning() << "Colour count doesn't match probability count";
        return;
    }

    emit preColoursAdded();

    for (int i = 0; i < colours.size(); ++i) {
        const auto colour = colours.at(i);
        const SwatchColour swatchColour(QString(), colour);
        if (mColours.contains(swatchColour))
            continue;

        mColours.append(swatchColour);
        mProbabilities.append(probabilities.at(i));
    }

    calculateProbabilitySum();

    emit postColoursAdded();
}

QVector<qreal> ProbabilitySwatch::probabilities() const
{
    return mProbabilities;
}

bool ProbabilitySwatch::hasNonZeroProbabilitySum() const
{
    return !qFuzzyIsNull(mProbabilitySum);
}

void ProbabilitySwatch::doAddColour(const QString &name, const QColor &colour)
{
    const SwatchColour swatchColour = SwatchColour(name, colour);
    if (mColours.contains(swatchColour))
        return;

    emit preColourAdded();

    mColours.append(swatchColour);
    mProbabilities.append(1);

    calculateProbabilitySum();

    emit postColourAdded();
}

void ProbabilitySwatch::doAddColours(const QVector<QColor> &colours)
{
    emit preColoursAdded();

    for (const auto colour : colours) {
        const SwatchColour swatchColour(QString(), colour);
        if (mColours.contains(swatchColour))
            continue;

        mColours.append(swatchColour);
        mProbabilities.append(1);
    }

    calculateProbabilitySum();

    emit postColoursAdded();
}

void ProbabilitySwatch::doRemoveColour(int index)
{
    emit preColourRemoved(index);

    mColours.removeAt(index);
    mProbabilities.removeAt(index);

    calculateProbabilitySum();

    emit postColourRemoved();
}

void ProbabilitySwatch::doCopy(const Swatch &other)
{
    const auto &otherSwatch = static_cast<const ProbabilitySwatch&>(other);

    emit preImported();

    mColours = otherSwatch.mColours;
    mProbabilities = otherSwatch.mProbabilities;

    calculateProbabilitySum();

    emit postImported();
}

bool ProbabilitySwatch::doRead(const QJsonObject &json, QString &errorMessage)
{
    if (!json.contains("colours")) {
        errorMessage = QLatin1String("No colours property");
        return false;
    }

    mColours.clear();
    mProbabilities.clear();

    const QJsonArray colourArray = json.value("colours").toArray();
    for (int i = 0; i < colourArray.size(); ++i) {
        const QJsonObject colourJsonObject = colourArray.at(i).toObject();
        SwatchColour colour;
        if (!colour.read(colourJsonObject, errorMessage))
            return false;

        const QJsonValue probabilityJsonValue = colourJsonObject.value("probability");
        if (probabilityJsonValue.isUndefined()) {
            errorMessage = QString::fromLatin1("Colour at index %1 has no \"probability\" key").arg(i);
            return false;
        }

        const qreal probability = probabilityJsonValue.toDouble();

        mColours.append(colour);
        mProbabilities.append(probability);
    }

    calculateProbabilitySum();

    return true;
}

void ProbabilitySwatch::doWrite(QJsonObject &json) const
{
    QJsonArray colourArray;
    for (int i = 0; i < mColours.size(); ++i) {
        const SwatchColour &swatchColour = mColours.at(i);

        QJsonObject colourObject;
        swatchColour.write(colourObject);
        colourObject["probability"] = mProbabilities.at(i);

        colourArray.append(colourObject);
    }
    json["colours"] = colourArray;
}

void ProbabilitySwatch::calculateProbabilitySum()
{
    mProbabilitySum = 0;
    for (const qreal probability : qAsConst(mProbabilities))
        mProbabilitySum += probability;
    emit nonZeroProbabilitySumChanged();
}
