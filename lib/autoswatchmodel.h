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

#ifndef AUTOSWATCHMODEL_H
#define AUTOSWATCHMODEL_H

#include <QAbstractListModel>
#include <QColor>
#include <QImage>
#include <QQmlEngine>
#include <QThread>
#include <QVector>

#include "slate-global.h"

class ImageLayer;
class ImageCanvas;

class AutoSwatchWorker : public QObject
{
    Q_OBJECT

public:
    AutoSwatchWorker(QObject *parent = nullptr);
    ~AutoSwatchWorker() override;

    Q_INVOKABLE void findUniqueColours(const QImage &image);

signals:
    void errorOccurred(const QString &errorMessage);
    void foundAllUniqueColours(const QVector<QColor> &colours);
};

class SLATE_EXPORT AutoSwatchModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(ImageCanvas *canvas READ canvas WRITE setCanvas NOTIFY canvasChanged)
    Q_PROPERTY(bool findingUniqueColours READ isFindingUniqueColours NOTIFY findingUniqueColoursChanged)
    Q_PROPERTY(QString failureMessage READ failureMessage NOTIFY failureMessageChanged)
    QML_ELEMENT
    Q_MOC_INCLUDE("imagecanvas.h")

public:
    enum {
        ColourRole = Qt::UserRole,
        ColourHexNameRole
    };

    explicit AutoSwatchModel(QObject *parent = nullptr);
    ~AutoSwatchModel() override;

    ImageCanvas *canvas() const;
    void setCanvas(ImageCanvas *canvas);

    bool isFindingUniqueColours() const;

    QString failureMessage() const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

signals:
    void canvasChanged();
    void findingUniqueColoursChanged();
    void failureMessageChanged();

private slots:
    void onProjectChanged();
    void onProjectClosed();
    void updateColours();
    void onFoundAllUniqueColours(const QVector<QColor> &colours);
    void onErrorOccurred(const QString &errorMessage);

private:
    void setFindingUniqueColours(bool findingUniqueColours);

    void setFailureMessage(const QString &message);

    ImageCanvas *mCanvas;
    QVector<QColor> mColours;

    AutoSwatchWorker mAutoSwatchWorker;
    QThread mAutoSwatchWorkerThread;
    bool mFindingUniqueColours = false;
    QString mFailureMessage;
};

#endif // AUTOSWATCHMODEL_H
