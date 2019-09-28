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

static const int maxAutoSwatchImageDimensionInPixels = 2048;
static const int maxUniqueColours = 10000;

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
        emit errorOccurred(tr("Cannot find unique colours in the image because it is null."));
        return;
    }

    QVector<QColor> uniqueColours;
    const Utils::FindUniqueColoursResult result = Utils::findUniqueColours(image, maxUniqueColours, uniqueColours);
    if (result == Utils::MaximumUniqueColoursExceeded) {
        // There was an actual error that the user should know about.
        emit errorOccurred(tr("Exceeded maximum unique colours (%1) supported by the auto swatch feature.")
            .arg(maxUniqueColours));
    } else {
        // Regardless of whether we succeeded or our thread was interrupted,
        // we consider it a success. If the thread was interrupted, we'll just have no colours.
        emit foundAllUniqueColours(uniqueColours);
    }
}

AutoSwatchModel::AutoSwatchModel(QObject *parent) :
    QAbstractListModel(parent),
    mCanvas(nullptr)
{
    mAutoSwatchWorker.moveToThread(&mAutoSwatchWorkerThread);

    connect(&mAutoSwatchWorker, &AutoSwatchWorker::foundAllUniqueColours,
        this, &AutoSwatchModel::onFoundAllUniqueColours);
    connect(&mAutoSwatchWorker, &AutoSwatchWorker::errorOccurred,
        this, &AutoSwatchModel::onErrorOccurred);
}

AutoSwatchModel::~AutoSwatchModel()
{
    qCDebug(lcAutoSwatchModel) << "destructing" << this << "...";

    qCDebug(lcAutoSwatchModel) << "calling requestInterruption() on auto swatch worker thread...";
    mAutoSwatchWorkerThread.requestInterruption();
    qCDebug(lcAutoSwatchModel) << ".. called requestInterruption() on auto swatch worker thread";
    qCDebug(lcAutoSwatchModel) << "calling quit() on auto swatch worker thread...";
    mAutoSwatchWorkerThread.quit();
    qCDebug(lcAutoSwatchModel) << ".. called quit() on auto swatch worker thread";
    qCDebug(lcAutoSwatchModel) << "calling wait() on auto swatch worker thread...";
    mAutoSwatchWorkerThread.wait();
    qCDebug(lcAutoSwatchModel) << "... called wait() on auto swatch worker thread";

    qCDebug(lcAutoSwatchModel) << "... destructed";
}

ImageCanvas *AutoSwatchModel::canvas() const
{
    return mCanvas;
}

void AutoSwatchModel::setCanvas(ImageCanvas *canvas)
{
    qCDebug(lcAutoSwatchModel) << "canvas changed to" << canvas;

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

bool AutoSwatchModel::isFindingUniqueColours() const
{
    return mFindingUniqueColours;
}

void AutoSwatchModel::setFindingUniqueColours(bool findingUniqueColours)
{
    if (findingUniqueColours == mFindingUniqueColours)
        return;

    mFindingUniqueColours = findingUniqueColours;
    emit findingUniqueColoursChanged();
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
    qCDebug(lcAutoSwatchModel).nospace() << "project changed to " << mCanvas->project()
        << "; requesting interruption of auto swatch thread...";
    mAutoSwatchWorkerThread.requestInterruption();
    // Apparently interruption doesn't actually quit it, so we do that ourselves.
    mAutoSwatchWorkerThread.quit();
    mAutoSwatchWorkerThread.wait();
    qCDebug(lcAutoSwatchModel) << "... auto swatch thread interrupted";

    if (mCanvas->project()) {
        connect(mCanvas->project()->undoStack(), &QUndoStack::indexChanged,
            this, &AutoSwatchModel::updateColours);
        // onProjectChanged() is not called when an existing project is closed,
        // as the canvas type won't change until the next project type is actually different.
        // That is why we can't just rely on onProjectChanged() to do our cleanup in.
        connect(mCanvas->project(), &Project::projectClosed,
            this, &AutoSwatchModel::onProjectClosed);

        // Force population.
        updateColours();
    }
}

void AutoSwatchModel::onProjectClosed()
{
    qCDebug(lcAutoSwatchModel) << "project closed; requesting interruption of auto swatch thread...";
    // Let the algorithm know that the thread it's running on has been interrupted.
    mAutoSwatchWorkerThread.requestInterruption();
    // Apparently interruption doesn't actually quit it, so we do that ourselves.
    mAutoSwatchWorkerThread.quit();
    mAutoSwatchWorkerThread.wait();
    qCDebug(lcAutoSwatchModel) << "... auto swatch thread interrupted";

    // Clear the colours.
    updateColours();
}

void AutoSwatchModel::updateColours()
{
    setFailureMessage(QString());

    // The index of the undo stack can be set in its destructor,
    // so we need to account for that here.
    if (mCanvas && mCanvas->project() && mCanvas->project()->hasLoaded()) {
        const QSize imageSize = mCanvas->project()->exportedImage().size();
        if (imageSize.width() * imageSize.height() > maxAutoSwatchImageDimensionInPixels * maxAutoSwatchImageDimensionInPixels) {
            setFailureMessage(tr("Exceeded maximum image dimensions (%1 x %1) supported by the auto swatch feature.")
                .arg(maxAutoSwatchImageDimensionInPixels));
            return;
        }

        qCDebug(lcAutoSwatchModel) << "starting auto swatch thread to find unique swatches...";
        setFindingUniqueColours(true);

        mAutoSwatchWorkerThread.start();

        const bool invokeSucceeded = QMetaObject::invokeMethod(&mAutoSwatchWorker, "findUniqueColours",
            Qt::QueuedConnection, Q_ARG(QImage, mCanvas->project()->exportedImage()));
        Q_ASSERT(invokeSucceeded);
    } else {
        qCDebug(lcAutoSwatchModel) << "no canvas/project; clearing model";

        setFindingUniqueColours(false);

        beginResetModel();
        mColours.clear();
        endResetModel();
    }
}

void AutoSwatchModel::onFoundAllUniqueColours(const QVector<QColor> &colours)
{
    qCDebug(lcAutoSwatchModel) << "auto swatch thread finished finding"
        << colours.size() << "unique swatches; resetting model...";

    beginResetModel();

    mColours = colours;

    endResetModel();

    qCDebug(lcAutoSwatchModel) << "... reset model";

    setFindingUniqueColours(false);

    mAutoSwatchWorkerThread.quit();
    mAutoSwatchWorkerThread.wait();
}

void AutoSwatchModel::onErrorOccurred(const QString &errorMessage)
{
    setFailureMessage(errorMessage);
    setFindingUniqueColours(false);

    mAutoSwatchWorkerThread.quit();
    mAutoSwatchWorkerThread.wait();
}
