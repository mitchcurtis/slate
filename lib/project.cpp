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

#include "project.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QImage>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLoggingCategory>
#include <QMetaEnum>

#include "applicationsettings.h"
#include "imageutils.h"
#include "qtutils.h"

Q_LOGGING_CATEGORY(lcProject, "app.project")
Q_LOGGING_CATEGORY(lcProjectGuides, "app.project.guides")
Q_LOGGING_CATEGORY(lcProjectNotes, "app.project.notes")
Q_LOGGING_CATEGORY(lcProjectLifecycle, "app.project.lifecycle")

Project::Project() :
    mSettings(nullptr),
    mFromNew(false),
    mUsingTempImage(false),
    mLivePreviewActive(false),
    mCurrentLivePreviewModification(LivePreviewModification::None),
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

QVersionNumber Project::creationVersion() const
{
    return mCreationVersion;
}

QVersionNumber Project::modificationVersion() const
{
    return mModificationVersion;
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

    if (objectName().isEmpty())
        setObjectName(typeString() + QLatin1Char('-') + mUrl.toString());

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

    mCreationVersion = QVersionNumber::fromString(qApp->applicationVersion());

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
    mLivePreviewActive = false;
    mUndoStack.clear();
    mUiState.reset(QVariantMap());

    doClose();

    qCDebug(lcProject) << "closed project";
}

bool Project::save()
{
    if (mFromNew) {
        Q_ASSERT_X(mUrl.isEmpty(), Q_FUNC_INFO, "New projects must have a valid URL to save to");
    }

    return saveAs(mUrl);
}

bool Project::saveAs(const QUrl &url)
{
    emit preProjectSaved();

    if (!hasLoaded()) {
        error(QLatin1String("Internal error: cannot save project as none has been loaded"));
        return false;
    }

    if (url.isEmpty()) {
        error(QLatin1String("Internal error: cannot save project with empty URL"));
        return false;
    }

    if (!doSaveAs(url))
        return false;

    emit postProjectSaved();
    return true;
}

void Project::revert()
{
    qCDebug(lcProject) << "reverting changes...";

    if (!hasLoaded())
        return;

    clearChanges();

    qCDebug(lcProject) << "... reverted changes";
}

void Project::importSwatch(SwatchImportFormat format, const QUrl &swatchUrl)
{
    const QString filePath = swatchUrl.toLocalFile();
    if (!QFileInfo::exists(filePath)) {
        error(QString::fromLatin1("Swatch file does not exist:\n\n%1").arg(filePath));
        return;
    }

    QFile swatchFile(filePath);
    if (!swatchFile.open(QIODevice::ReadOnly)) {
        error(QString::fromLatin1("Failed to open swatch file:\n\n%1").arg(filePath));
        return;
    }

    if (format == SlateSwatch) {
        QJsonDocument jsonDoc = QJsonDocument::fromJson(swatchFile.readAll());
        QJsonObject rootJson = jsonDoc.object();
        readJsonSwatch(rootJson, ErrorOutOnSerialisationFailures);
    } else if (format == PaintNetSwatch) {
        readPaintNetSwatch(swatchFile);
    }
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
    writeJsonSwatch(rootJson);

    QJsonDocument jsonDoc(rootJson);
    const qint64 bytesWritten = jsonFile.write(jsonDoc.toJson());
    if (bytesWritten == -1)
        error(QString::fromLatin1("Failed to write to swatch file:\n\n%1").arg(jsonFile.errorString()));
}

void Project::beginLivePreview()
{
    qWarning() << "This project type doesn't support live preview!";
}

void Project::endLivePreview(LivePreviewModificationAction /*modificationAction*/)
{
    qWarning() << "This project type doesn't support live preview!";
}

bool Project::warnIfLivePreviewNotActive(const QString &actionName) const
{
    if (!mLivePreviewActive) {
        qWarning() << "Cannot" << actionName << "as live preview isn't active";
        return true;
    }
    return false;
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

bool Project::doSaveAs(const QUrl &)
{
    return false;
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

    QImage tempImage = ImageUtils::filledImage(width, height, colour);

    const QString dateString = QDateTime::currentDateTime().toString(QLatin1String("hh-mm-ss-zzz"));
    const QString fileName = QString::fromLatin1("%1/tmp-image-%2.png").arg(mTempDir.path(), dateString);
    if (!tempImage.save(fileName)) {
        error(QString::fromLatin1("Failed to save temporary image to %1").arg(fileName));
        return QUrl();
    }

    qCDebug(lcProject) << "Successfully created temporary image:" << fileName;
    mUsingTempImage = true;
    if (objectName().isEmpty())
        setObjectName(typeString() + QLatin1Char('-') + fileName);
    return QUrl::fromLocalFile(fileName);
}

void Project::readVersionNumbers(const QJsonObject &projectJson)
{
    // TODO: call error() if this is missing in v0.11.0, and advise to use v0.10.0 to convert the project so that
    // it's readable with newer versions
    if (projectJson.contains("creationVersion")) {
        mCreationVersion = QVersionNumber::fromString(projectJson.value("creationVersion").toString());
        mModificationVersion = QVersionNumber::fromString(projectJson.value("modificationVersion").toString());
    }
}

void Project::writeVersionNumbers(QJsonObject &projectJson)
{
    projectJson["creationVersion"] = mCreationVersion.toString();

    mModificationVersion = QVersionNumber::fromString(qApp->applicationVersion());
    projectJson["modificationVersion"] = mModificationVersion.toString();
}

void Project::readGuides(const QJsonObject &projectJson)
{
    mGuides.clear();

    bool foundDuplicateGuides = false;

    QJsonArray guidesArray = projectJson.value(QLatin1String("guides")).toArray();
    for (int i = 0; i < guidesArray.size(); ++i) {
        QJsonObject guideObject = guidesArray.at(i).toObject();
        const int position = guideObject.value(QLatin1String("position")).toInt();
        const Qt::Orientation orientation = static_cast<Qt::Orientation>(
            guideObject.value(QLatin1String("orientation")).toInt());
        const Guide guide(position, orientation);
        if (!mGuides.contains(guide))
            mGuides.append(guide);
        else
            foundDuplicateGuides = true;
    }

    if (foundDuplicateGuides)
        qWarning() << "Project contains duplicate guides; they will be removed";
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

void Project::readNotes(const QJsonObject &projectJson)
{
    mNotes.clear();

    QJsonArray notesArray = projectJson.value(QLatin1String("notes")).toArray();
    for (int i = 0; i < notesArray.size(); ++i) {
        QJsonObject noteObject = notesArray.at(i).toObject();
        const int positionX = noteObject.value(QLatin1String("positionX")).toInt();
        const int positionY = noteObject.value(QLatin1String("positionY")).toInt();
        const QString text = noteObject.value(QLatin1String("text")).toString();
        mNotes.append(Note(QPoint(positionX, positionY), text));
    }
}

void Project::writeNotes(QJsonObject &projectJson) const
{
    QJsonArray notesArray;
    for (int i = 0; i < mNotes.size(); ++i) {
        QJsonObject noteObject;
        const Note note = mNotes.at(i);
        noteObject[QLatin1String("positionX")] = note.position().x();
        noteObject[QLatin1String("positionY")] = note.position().y();
        noteObject[QLatin1String("text")] = note.text();
        notesArray.append(noteObject);
    }
    projectJson[QLatin1String("notes")] = notesArray;
}

void Project::readUiState(const QJsonObject &projectJson)
{
    QVariantMap uiState;
    // Older versions (< 0.8.0) didn't have uiState.
    if (projectJson.contains("uiState"))
        uiState = projectJson.value("uiState").toObject().toVariantMap();

    mUiState.reset(uiState);
}

void Project::writeUiState(QJsonObject &projectJson)
{
    projectJson["uiState"] = QJsonObject::fromVariantMap(mUiState.map());
}

bool Project::readJsonSwatch(const QJsonObject &projectJson, SerialisationFailurePolicy serialisationFailurePolicy)
{
    Swatch swatch;
    QString errorMessage;

    const QJsonObject swatchJson = projectJson[QLatin1String("swatch")].toObject();
    const bool readSuccessfully = swatch.read(swatchJson, errorMessage);
    if (!readSuccessfully && serialisationFailurePolicy == ErrorOutOnSerialisationFailures) {
        error(QLatin1String("Failed to read Slate (JSON) swatch: ") + errorMessage);
        return false;
    }

    mSwatch.copy(swatch);
    return true;
}

void Project::writeJsonSwatch(QJsonObject &projectJson) const
{
    QJsonObject swatchObject;
    mSwatch.write(swatchObject);
    projectJson[QLatin1String("swatch")] = swatchObject;
}

bool Project::readPaintNetSwatch(QFile &file)
{
    Swatch newSwatch;

    for (int lineNumber = 1; !file.atEnd(); ++lineNumber) {
        // Comments could conceivably contain non-latin1 characters.
        const QString line = QString::fromUtf8(file.readLine()).trimmed();

        // Ignore comments.
        if (line.startsWith(QLatin1Char(';')))
            continue;

        const QString colourString = QLatin1Char('#') + line;
        if (!QColor::isValidColor(colourString)) {
            error(QString::fromLatin1("Invalid colour %1 at line %2 of paint.net swatch file %3")
                .arg(line).arg(lineNumber).arg(file.fileName()));
            return false;
        }

        const QColor colour(colourString);
        newSwatch.addColour(QString(), colour);
    }

    mSwatch.copy(newSwatch);
    return true;
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

void Project::addGuides(const QVector<Guide> &guides)
{
    qCDebug(lcProjectGuides) << "addGuides called with:\n" << guides
        << "\nexisting guides:\n" << mGuides;

    const int appendIndex = mGuides.size();
    const auto uniqueGuides = QtUtils::uniqueValues(guides);
    emit preGuidesAdded(appendIndex, uniqueGuides.size());

    mGuides += uniqueGuides;

    if (uniqueGuides.size() > 0)
        emit postGuidesAdded();
}

void Project::moveGuide(const Guide &guide, int to)
{
    const int guideIndex = mGuides.indexOf(guide);
    if (guideIndex == -1)
        return;

    mGuides[guideIndex].setPosition(to);

    emit guideMoved(guideIndex);
}

void Project::removeGuides(const QVector<Guide> &guides)
{
    emit postGuidesRemoved();

    // TODO: make this more strict so that we can conditionally emit signals
    for (const auto guide : guides)
        mGuides.removeOne(guide);

    emit postGuidesRemoved();
}

bool Project::hasNotes() const
{
    return !mNotes.isEmpty();
}

QVector<Note> Project::notes() const
{
    return mNotes;
}

void Project::addNote(const Note &note)
{
    if (mNotes.contains(note))
        return;

    qCDebug(lcProjectNotes) << "adding note" << note;
    mNotes.append(note);

    emit notesChanged();
}

void Project::modifyNote(int noteIndex, const Note &note)
{
    if (!isValidNoteIndex(noteIndex)) {
        qWarning() << "Cannot modify note at index" << noteIndex << "as it is an invalid index!";
        return;
    }

    qCDebug(lcProjectNotes) << "modifying note at index" << noteIndex << "with" << note;
    mNotes[noteIndex] = note;

    // This function assumes that the caller ensured that something actually changed.
    emit notesChanged();
}

void Project::removeNote(const Note &note)
{
    qCDebug(lcProjectNotes) << "removing note" << note;
    if (mNotes.removeOne(note))
        emit notesChanged();
}

QPoint Project::notePositionAtIndex(int index) const
{
    if (!isValidNoteIndex(index)) {
        qWarning() << "Note index" << index << "is invalid!";
        return QPoint();
    }

    return mNotes.at(index).position();
}

QString Project::noteTextAtIndex(int index) const
{
    if (!isValidNoteIndex(index)) {
        qWarning() << "Note index" << index << "is invalid!";
        return QString();
    }

    return mNotes.at(index).text();
}

bool Project::isValidNoteIndex(int index) const
{
    return index >= 0 && index < mNotes.size();
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
    QDebugStateSaver saver(debug);
    if (!command)
        return debug << "QUndoCommand(0x0)";

    debug.nospace() << "(QUndoCommand id=" << command->id()
        << " text=" << command->text()
        << ")";
    return debug.space();
}

void Project::addChange(UndoCommand *undoCommand)
{
    qCDebug(lcProject) << "adding change" << undoCommand;

    const bool modifiedContents = undoCommand->modifiesContents();

    mUndoStack.push(undoCommand);

    if (modifiedContents)
        emit contentsModified();
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
