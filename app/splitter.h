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

#ifndef SPLITTER_H
#define SPLITTER_H

#include <QObject>

class TileCanvas;

class Splitter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(qreal position READ position NOTIFY positionChanged FINAL)
    Q_PROPERTY(int width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(bool pressed READ isPressed WRITE setPressed NOTIFY pressedChanged FINAL)
    Q_PROPERTY(bool hovered READ isHovered WRITE setHovered NOTIFY hoveredChanged FINAL REVISION 1)

public:
    explicit Splitter(TileCanvas *tileCanvas);
    ~Splitter();

    bool isEnabled() const;
    void setEnabled(bool enabled);

    qreal position() const;
    void setPosition(qreal position);

    int width() const;
    void setWidth(int width);

    bool isPressed() const;
    void setPressed(bool pressed);

    bool isHovered() const;
    void setHovered(bool hovered);

signals:
    void enabledChanged();
    void positionChanged();
    void widthChanged();
    void pressedChanged();
    void hoveredChanged();

private:
    TileCanvas *mTileCanvas;
    bool mEnabled;
    qreal mPosition;
    int mWidth;
    bool mPressed;
    bool mHovered;
};

#endif // SPLITTER_H
