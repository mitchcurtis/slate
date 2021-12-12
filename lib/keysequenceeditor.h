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

#ifndef KEYSEQUENCEEDITOR_H
#define KEYSEQUENCEEDITOR_H

#include <QQuickItem>
#include <QKeySequence>

#include "slate-global.h"

class QKeyEvent;

class SLATE_EXPORT KeySequenceEditor : public QQuickItem
{
    Q_OBJECT
    // The sequence that is current in the settings (e.g. is current when the settings dialog was opened).
    Q_PROPERTY(QString originalSequence READ originalSequence WRITE setOriginalSequence NOTIFY originalSequenceChanged)
    // The last sequence that was successfully entered.
    Q_PROPERTY(QString newSequence READ newSequence NOTIFY newSequenceChanged)
    Q_PROPERTY(QString displaySequence READ displaySequence NOTIFY displaySequenceChanged)
    Q_PROPERTY(bool hasChanged READ hasChanged NOTIFY hasChangedChanged)
    QML_ELEMENT

public:
    KeySequenceEditor();

    QString originalSequence() const;
    void setOriginalSequence(const QString &originalSequence);

    QString newSequence() const;

    QString displaySequence() const;

    bool hasChanged() const;

public slots:
    void reset();

signals:
    void originalSequenceChanged();
    void newSequenceChanged();
    void displaySequenceChanged();
    void hasChangedChanged();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

private:
    void setCurrentSequence(const QString &currentSequence);
    void setNewSequence(const QString &newSequence);

    void resetKeysPressed();

    void accept();
    void cancel();
    bool validate(const QKeySequence &sequence) const;

    QKeySequence mOriginalSequence;
    QKeySequence mCurrentSequence;
    QKeySequence mNewSequence;
    int mCurrentKeyIndex;
    QVector<int> mKeysPressed;
};

#endif // KEYSEQUENCEEDITOR_H
