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

#ifndef PROBABILITYSWATCH_H
#define PROBABILITYSWATCH_H

#include <QQmlEngine>
#include <QVector>

#include "slate-global.h"
#include "swatch.h"

class QJsonObject;

class SLATE_EXPORT ProbabilitySwatch : public Swatch
{
    Q_OBJECT
    Q_PROPERTY(bool nonZeroProbabilitySum READ hasNonZeroProbabilitySum NOTIFY nonZeroProbabilitySumChanged)
    QML_ANONYMOUS

public:
    explicit ProbabilitySwatch(QObject *parent = nullptr);

    Q_INVOKABLE void setProbability(int index, qreal probability);

    void addColoursWithProbabilities(const QVector<QColor> &colours, const QVector<qreal> &probabilities);

    QVector<qreal> probabilities() const;

    // True if the sum of our probabilities is greater than zero.
    // If the sum is zero, no colours can be chosen, which doesn't make sense.
    bool hasNonZeroProbabilitySum() const;

signals:
    void probabilityChanged(int index);
    void nonZeroProbabilitySumChanged();

protected:
    virtual void doAddColour(const QString &name, const QColor &colour);
    virtual void doAddColours(const QVector<QColor> &colours);
    virtual void doRemoveColour(int index);
    virtual void doCopy(const Swatch &other);
    virtual bool doRead(const QJsonObject &json, QString &errorMessage);
    virtual void doWrite(QJsonObject &json) const;

private:
    void calculateProbabilitySum();

    QVector<qreal> mProbabilities;
    qreal mProbabilitySum;
};

#endif // PROBABILITYSWATCH_H
