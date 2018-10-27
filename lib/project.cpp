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

#include "project.h"

#include <QDateTime>
#include <QImage>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLoggingCategory>
#include <QMetaEnum>

#include "applicationsettings.h"

Q_LOGGING_CATEGORY(lcProject, "app.project")
Q_LOGGING_CATEGORY(lcProjectLifecycle, "app.project.lifecycle")

Project::Project() :
    mSettings(nullptr),
    mFromNew(false),
    mUsingTempImage(false),
    mComposingMacro(false),
    mHadUnsavedChangesBeforeMacroBegan(false)
{
    connect(&mUndoStack, SIGNAL(cleanChanged(bool)), this, SIGNAL(unsavedChangesChanged()));
}

Project::Type Project::type() const
{
    return UnknownType;
}

QString Project::typeString() const
{
    return typeToString(type());
}

QString Project::typeToString(Project::Type type)
{
    QMetaEnum metaEnum = QMetaEnum::fromType<Project::Type>();
    return metaEnum.valueToKey(type);
}

QUrl Project::url() const
{
    return mUrl;
}

void Project::setUrl(const QUrl &url)
{
    if (url == mUrl)
        return;

    const bool wasLoaded = hasLoaded();

    mUrl = url;

    if (wasLoaded != hasLoaded()) {
        emit loadedChanged();
    }

    emit urlChanged();
}

QString Project::displayUrl() const
{
    return mUrl.toDisplayString(QUrl::PreferLocalFile);
}

QUrl Project::dirUrl() const
{
    QDir dir(mUrl.toLocalFile());
    if (!dir.cdUp())
        return QUrl();

    return QUrl::fromLocalFile(dir.path());
}

QString Project::fileBaseName() const
{
    return QFileInfo(mUrl.toLocalFile()).baseName();
}

bool Project::hasLoaded() const
{
    return mFromNew || mUrl.isValid();
}

bool Project::isNewProject() const
{
    return mFromNew;
}

void Project::setNewProject(bool newProject)
{
    if (newProject == mFromNew)
        return;

    const bool wasLoaded = hasLoaded();
    const bool couldSave = canSave();

    mFromNew = newProject;

    if (wasLoaded != hasLoaded()) {
        emit loadedChanged();
    }

    if (couldSave != canSave()) {
        emit canSaveChanged();
    }

    emit newProjectChanged();
}

bool Project::hasUnsavedChanges() const
{
    return !mUndoStack.isClean();
}

bool Project::canSave() const
{
    return !mComposingMacro && (isNewProject() || hasUnsavedChanges());
}

void Project::load(const QUrl &url)
{
    qCDebug(lcProject) << "loading project:" << url;

    close();

    doLoad(url);

    qCDebug(lcProject) << (hasLoaded() ? "loaded project" : "failed to load project");
}

void Project::close()
{
    if (!hasLoaded())
        return;

    qCDebug(lcProject) << "closing project:" << mUrl;

    setNewProject(false);
    setUrl(QUrl());
    mUndoStack.clear();
    mUiState.reset(QVariantMap());

    doClose();

    qCDebug(lcProject) << "closed project";
}

void Project::save()
{
    if (mFromNew) {
        Q_ASSERT_X(mUrl.isEmpty(), Q_FUNC_INFO, "New projects must have a valid URL to save to");
    }

    saveAs(mUrl);
}

void Project::saveAs(const QUrl &url)
{
    emit preProjectSaved();

    doSaveAs(url);
}

void Project::revert()
{
    qCDebug(lcProject) << "reverting changes...";

    if (!hasLoaded())
        return;

    clearChanges();

    qCDebug(lcProject) << "... reverted changes";
}

void Project::importSwatch(const QUrl &swatchUrl)
{
    const QString filePath = swatchUrl.toLocalFile();
    if (!QFileInfo::exists(filePath)) {
        error(QString::fromLatin1("Swatch file does not exist:\n\n%1").arg(filePath));
        return;
    }

    QFile jsonFile(filePath);
    if (!jsonFile.open(QIODevice::ReadOnly)) {
        error(QString::fromLatin1("Failed to open swatch file:\n\n%1").arg(filePath));
        return;
    }

    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonFile.readAll());
    QJsonObject rootJson = jsonDoc.object();
    readSwatch(rootJson, ErrorOutOnSerialisationFailures);
}

void Project::exportSwatch(const QUrl &swatchUrl)
{
    QFile jsonFile;
    const QString filePath = swatchUrl.toLocalFile();
    if (QFile::exists(filePath)) {
        jsonFile.setFileName(filePath);
        if (!jsonFile.open(QIODevice::WriteOnly)) {
            error(QString::fromLatin1("Failed to open swatch file:\n\n%1").arg(filePath));
            return;
        }
    } else {
        jsonFile.setFileName(filePath);
        if (!jsonFile.open(QIODevice::WriteOnly)) {
            error(QString::fromLatin1("Failed to create swatch file:\n\n%1").arg(filePath));
            return;
        }
    }

    QJsonObject rootJson;
    writeSwatch(rootJson);

    QJsonDocument jsonDoc(rootJson);
    const qint64 bytesWritten = jsonFile.write(jsonDoc.toJson());
    if (bytesWritten == -1)
        error(QString::fromLatin1("Failed to write to swatch file:\n\n%1").arg(jsonFile.errorString()));
}

void Project::error(const QString &message)
{
    qCDebug(lcProject) << "emitting errorOccurred with message" << message;
    emit errorOccurred(message);
}

void Project::doLoad(const QUrl &)
{
}

void Project::doClose()
{
}

void Project::doSaveAs(const QUrl &)
{
}

void Project::setComposingMacro(bool composingMacro, const QString &macroText)
{
    // If we're not composing a macro, we don't need to specify the text.
    Q_ASSERT(!composingMacro || (composingMacro && !macroText.isEmpty()));

    if (composingMacro == mComposingMacro)
        return;

    const bool couldSave = canSave();
    mComposingMacro = composingMacro;
    mCurrentlyComposingMacroText = macroText;
    if (couldSave != canSave()) {
        emit canSaveChanged();
    }
}

QUrl Project::createTemporaryImage(int width, int height, const QColor &colour)
{
    if (!mTempDir.isValid()) {
        error(QString::fromLatin1("Failed to create temporary image directory: %1").arg(mTempDir.errorString()));
        return QUrl();
    }

    QImage tempImage(width, height, QImage::Format_ARGB32_Premultiplied);
    tempImage.fill(colour);

    const QString dateString = QDateTime::currentDateTime().toString(QLatin1String("hh-mm-ss-zzz"));
    const QString fileName = QString::fromLatin1("%1/tmp-image-%2.png").arg(mTempDir.path()).arg(dateString);
    if (!tempImage.save(fileName)) {
        error(QString::fromLatin1("Failed to save temporary image to %1").arg(fileName));
        return QUrl();
    }

    qCDebug(lcProject) << "Successfully created temporary image:" << fileName;
    mUsingTempImage = true;
    return QUrl::fromLocalFile(fileName);
}

void Project::readGuides(const QJsonObject &projectJson)
{
    mGuides.clear();

    QJsonArray guidesArray = projectJson.value(QLatin1String("guides")).toArray();
    for (int i = 0; i < guidesArray.size(); ++i) {
        QJsonObject guideObject = guidesArray.at(i).toObject();
        const int position = guideObject.value(QLatin1String("position")).toInt();
        const Qt::Orientation orientation = static_cast<Qt::Orientation>(
            guideObject.value(QLatin1String("orientation")).toInt());
        mGuides.append(Guide(position, orientation));
    }
}

void Project::writeGuides(QJsonObject &projectJson) const
{
    QJsonArray guidesArray;
    for (int i = 0; i < mGuides.size(); ++i) {
        QJsonObject guideObject;
        const Guide guide = mGuides.at(i);
        guideObject[QLatin1String("position")] = guide.position();
        guideObject[QLatin1String("orientation")] = guide.orientation();
        guidesArray.append(guideObject);
    }
    projectJson[QLatin1String("guides")] = guidesArray;
}

void Project::readUiState(const QJsonObject &projectJson)
{
    QVariantMap uiState;
    // Older versions (<= 0.4.0) didn't have uiState.
    if (projectJson.contains("uiState"))
        uiState = projectJson.value("uiState").toObject().toVariantMap();

    mUiState.reset(uiState);
}

void Project::writeUiState(QJsonObject &projectJson)
{
    projectJson["uiState"] = QJsonObject::fromVariantMap(mUiState.map());
}

bool Project::readSwatch(const QJsonObject &projectJson, SerialisationFailurePolicy serialisationFailurePolicy)
{
    Swatch swatch;
    QString errorMessage;

    const QJsonObject swatchJson = projectJson[QLatin1String("swatch")].toObject();
    const bool readSuccessfully = swatch.read(swatchJson, errorMessage);
    if (!readSuccessfully && serialisationFailurePolicy == ErrorOutOnSerialisationFailures) {
        error(QLatin1String("Failed to read swatch: ") + errorMessage);
        return false;
    }

    mSwatch.copy(swatch);
    return true;
}

void Project::writeSwatch(QJsonObject &projectJson) const
{
    QJsonObject swatchObject;
    mSwatch.write(swatchObject);
    projectJson[QLatin1String("swatch")] = swatchObject;
}

ApplicationSettings *Project::settings() const
{
    return mSettings;
}

void Project::setSettings(ApplicationSettings *settings)
{
    if (settings == mSettings)
        return;

    mSettings = settings;
    emit settingsChanged();
}

QJsonObject *Project::cachedProjectJson()
{
    return &mCachedProjectJson;
}

SerialisableState *Project::uiState()
{
    return &mUiState;
}

QSize Project::size() const
{
    return QSize(0, 0);
}

void Project::setSize(const QSize &)
{
}

QRect Project::bounds() const
{
    return QRect();
}

int Project::widthInPixels() const
{
    return 0;
}

int Project::heightInPixels() const
{
    return 0;
}

QVector<Guide> Project::guides() const
{
    return mGuides;
}

void Project::addGuide(const Guide &guide)
{
    if (mGuides.contains(guide))
        return;

    mGuides.append(guide);

    emit guidesChanged();
}

void Project::moveGuide(const Guide &guide, int to)
{
    auto it = std::find(mGuides.begin(), mGuides.end(), guide);
    if (it == mGuides.end())
        return;

    it->setPosition(to);

    emit guidesChanged();
}

void Project::removeGuide(const Guide &guide)
{
    if (mGuides.removeOne(guide)) {
        emit guidesChanged();
    }
}

Swatch *Project::swatch()
{
    return &mSwatch;
}

const Swatch *Project::swatch() const
{
    return &mSwatch;
}

int Project::currentLayerIndex() const
{
    return -1;
}

QImage Project::exportedImage() const
{
    return QImage();
}

QUndoStack *Project::undoStack()
{
    return &mUndoStack;
}

bool Project::isComposingMacro() const
{
    return mComposingMacro;
}

void Project::beginMacro(const QString &text)
{
    // ImageCanvas has the concept of a selection. For simplicity, rather
    // than disable the parts of the UI that shouldn't be accessible while the
    // canvas has a selection, we choose to instead call clearOrConfirmSelection()
    // right before performing the relevant command that the user just carried out.
    // However, projects don't want or need to know about ImageCanvas, so we emit
    // this signal so that they have the chance to call clearOrConfirmSelection()
    // without us having to know about them.
    // This was added to fix https://github.com/mitchcurtis/slate/issues/69.
    emit aboutToBeginMacro(text);

    if (isComposingMacro()) {
        // For now we've been considering it OK to do the following:
        //
        // beginMacro("foo")
        // *add some command to the stack*
        // endMacro()
        //
        // If there is already a macro in progress, the command will just be added to it.
        // We might want to review this (and our usage of macros) in the future to ensure
        // that we're doing it correctly.
        qCDebug(lcProject).nospace() << "tried to begin macro " << text
            << ", but we're busy composing " << mCurrentlyComposingMacroText << " right now";
        return;
    }

    qCDebug(lcProject) << "beginning macro" << text;

    mHadUnsavedChangesBeforeMacroBegan = hasUnsavedChanges();
    mUndoStack.beginMacro(text);
    setComposingMacro(true, text);
}

void Project::endMacro()
{
    Q_ASSERT(mComposingMacro);

    qCDebug(lcProject) << "ending macro";

    mUndoStack.endMacro();
    // This handles the emission of the canSaveChanged signal.
    setComposingMacro(false);

    // It's not enough to rely on the cleanChanged signal to cause
    // our unchangedChangesSignal to be called, because cleanChanged
    // apparently does not get emitted when a macro ends. So, we do it ourselves here.
    if (hasUnsavedChanges() != mHadUnsavedChangesBeforeMacroBegan) {
        emit unsavedChangesChanged();
    }
}

// TODO: why aren't the individual classes' operators used?
QDebug operator<<(QDebug debug, const QUndoCommand *command)
{
    debug.nospace() << "(UndoCommand id=" << command->id()
        << " text=" << command->text()
        << ")";
    return debug.space();
}

void Project::addChange(QUndoCommand *undoCommand)
{
    qCDebug(lcProject) << "adding change" << undoCommand;
    mUndoStack.push(undoCommand);
}

void Project::clearChanges()
{
    const bool hadUnsavedChanges = hasUnsavedChanges();

    mUndoStack.setIndex(mUndoStack.cleanIndex());
    mHadUnsavedChangesBeforeMacroBegan = false;

    if (hasUnsavedChanges() != hadUnsavedChanges) {
        emit unsavedChangesChanged();
    }
}
