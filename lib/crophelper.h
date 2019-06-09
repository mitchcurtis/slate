/*
    Copyright 2017, Mitch Curtis

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

#ifndef CROPHELPER_H
#define CROPHELPER_H

#include <QObject>
#include <QRect>

class QMouseEvent;
class QHoverEvent;

class Project;

class CropHelper : public QObject
{
    Q_OBJECT
//    Q_PROPERTY(Project *project READ project WRITE setProject NOTIFY projectChanged)
    Q_PROPERTY(QRect cropArea READ cropArea NOTIFY cropAreaChanged)

public:
    CropHelper();
    ~CropHelper() override;

    QRect cropArea() const;
    void setCropArea(const QRect &cropArea);

signals:
    void cropAreaChanged();

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void hoverEnterEvent(QHoverEvent *event);
    void hoverMoveEvent(QHoverEvent *event);
    void hoverLeaveEvent(QHoverEvent *event);

private:
    Project *mProject;
    QRect mCropArea;
};

#endif // CROPHELPER_H
