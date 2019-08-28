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

#include "autoswatchmodel.h"

#include <QLoggingCategory>

#include "imagecanvas.h"
#include "project.h"
#include "utils.h"

Q_LOGGING_CATEGORY(lcAutoSwatchModel, "app.autoSwatchModel")

static const int maxAutoSwatchImagePixels = 400 * 400;

AutoSwatchWorker::AutoSwatchWorker(QObject *parent) :
    QObject(parent)
{
}

AutoSwatchWorker::~AutoSwatchWorker()
{
}

void AutoSwatchWorker::findUniqueColours(const QImage &image)
{
    if (image.isNull()) {
        emit foundAllUniqueColours(QVector<QColor>());
        return;
    }

    const QVector<QColor> colours = Utils::findUniqueColours(image);
    emit foundAllUniqueColours(colours);
}

AutoSwatchModel::AutoSwatchModel(QObject *parent) :
    QAbstractListModel(parent),
    mCanvas(nullptr)
{
    mAutoSwatchWorker.moveToThread(&mAutoSwatchWorkerThread);

    connect(&mAutoSwatchWorker, &AutoSwatchWorker::foundAllUniqueColours,
        this, &AutoSwatchModel::onFoundAllUniqueColours);
    connect(&mAutoSwatchWorker, &AutoSwatchWorker::foundAllUniqueColours,
        &mAutoSwatchWorkerThread, &QThread::quit);
}

AutoSwatchModel::~AutoSwatchModel()
{
    qCDebug(lcAutoSwatchModel) << "destructing" << this << "...";

    mAutoSwatchWorkerThread.quit();
    mAutoSwatchWorkerThread.wait();

    qCDebug(lcAutoSwatchModel) << "... destructed";
}

ImageCanvas *AutoSwatchModel::canvas() const
{
    return mCanvas;
}

void AutoSwatchModel::setCanvas(ImageCanvas *canvas)
{
    if (canvas == mCanvas)
        return;

    if (mCanvas) {
        mCanvas->disconnect(this);
    }

    beginResetModel();

    mColours.clear();
    mCanvas = canvas;

    endResetModel();
    emit canvasChanged();

    if (mCanvas) {
        connect(mCanvas, &ImageCanvas::projectChanged, this, &AutoSwatchModel::onProjectChanged);
        connect(mCanvas, &ImageCanvas::pasteSelectionConfirmed, this, &AutoSwatchModel::updateColours);

        if (mCanvas->project())
            onProjectChanged();
    }
}

bool AutoSwatchModel::isFindingSwatches() const
{
    return mFindingSwatches;
}

void AutoSwatchModel::setFindingSwatches(bool findingSwatches)
{
    if (findingSwatches == mFindingSwatches)
        return;

    mFindingSwatches = findingSwatches;
    emit findingSwatchesChanged();
}

QString AutoSwatchModel::failureMessage() const
{
    return mFailureMessage;
}

void AutoSwatchModel::setFailureMessage(const QString &message)
{
    if (message == mFailureMessage)
        return;

    mFailureMessage = message;
    emit failureMessageChanged();
}

QVariant AutoSwatchModel::data(const QModelIndex &index, int role) const
{
    if (!mCanvas || !mCanvas->project() || !checkIndex(index, CheckIndexOption::IndexIsValid))
        return QVariant();

    if (role == ColourRole) {
        const auto colour = mColours.at(index.row());
        return QVariant::fromValue(colour);
    } else if (role == ColourHexNameRole) {
        const auto colour = mColours.at(index.row());
        return QVariant::fromValue(colour.name(QColor::HexArgb));
    }

    return QVariant();
}

int AutoSwatchModel::rowCount(const QModelIndex &) const
{
    if (!mCanvas || !mCanvas->project())
        return 0;

    return mColours.size();
}

int AutoSwatchModel::columnCount(const QModelIndex &) const
{
    return 1;
}

QHash<int, QByteArray> AutoSwatchModel::roleNames() const
{
    QHash<int, QByteArray> names;
    names.insert(ColourRole, "colour");
    names.insert(ColourHexNameRole, "colourHexName");
    return names;
}

void AutoSwatchModel::onProjectChanged()
{
    if (mCanvas->project()) {
        connect(mCanvas->project()->undoStack(), &QUndoStack::indexChanged,
            this, &AutoSwatchModel::updateColours);

        // Force population.
        qCDebug(lcAutoSwatchModel) << "project changed; forcing auto swatch model population";
        updateColours();
    }
}

void AutoSwatchModel::updateColours()
{
    setFailureMessage(QString());

    // The index of the undo stack can be set in its destructor,
    // so we need to account for that here.
    if (mCanvas && mCanvas->project() && mCanvas->project()->hasLoaded()) {
        const QSize imageSize = mCanvas->project()->exportedImage().size();
        if (imageSize.width() * imageSize.height() > maxAutoSwatchImagePixels) {
            setFailureMessage(tr("Exceeded maximum image dimensions (400 x 400) supported by the auto swatch feature."));
            return;
        }

        qCDebug(lcAutoSwatchModel) << "starting auto swatch thread to find unique swatches...";
        setFindingSwatches(true);

        mAutoSwatchWorkerThread.start();

        const bool invokeSucceeded = QMetaObject::invokeMethod(&mAutoSwatchWorker, "findUniqueColours",
            Qt::QueuedConnection, Q_ARG(QImage, mCanvas->project()->exportedImage()));
        Q_ASSERT(invokeSucceeded);
    } else {
        qCDebug(lcAutoSwatchModel) << "no canvas/project; clearing model";

        setFindingSwatches(false);

        beginResetModel();
        mColours.clear();
        endResetModel();
    }
}

void AutoSwatchModel::onFoundAllUniqueColours(const QVector<QColor> &colours)
{
    beginResetModel();

    mColours = colours;

    endResetModel();

    setFindingSwatches(false);

    qCDebug(lcAutoSwatchModel) << "... auto swatch thread finished finding"
        << mColours.size() << "unique swatches";
}
