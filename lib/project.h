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

#ifndef PROJECT_H
#define PROJECT_H

#include <QImage>
#include <QJsonObject>
#include <QLoggingCategory>
#include <QObject>
#include <QSize>
#include <QTemporaryDir>
#include <QUrl>

#include <QUndoStack>

#include "guide.h"
#include "slate-global.h"
#include "swatch.h"

Q_DECLARE_LOGGING_CATEGORY(lcProject)
Q_DECLARE_LOGGING_CATEGORY(lcProjectLifecycle)

class ApplicationSettings;

class SLATE_EXPORT Project : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Type type READ type CONSTANT)
    Q_PROPERTY(QString typeString READ typeString CONSTANT)
    Q_PROPERTY(bool loaded READ hasLoaded NOTIFY loadedChanged)
    Q_PROPERTY(bool newProject READ isNewProject WRITE setNewProject NOTIFY newProjectChanged)
    Q_PROPERTY(bool unsavedChanges READ hasUnsavedChanges NOTIFY unsavedChangesChanged)
    Q_PROPERTY(bool canSave READ canSave NOTIFY canSaveChanged)
    Q_PROPERTY(QUrl url READ url NOTIFY urlChanged)
    Q_PROPERTY(QUrl dirUrl READ dirUrl NOTIFY urlChanged)
    Q_PROPERTY(QString displayUrl READ displayUrl NOTIFY urlChanged)
    Q_PROPERTY(QSize size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(QUndoStack *undoStack READ undoStack CONSTANT)
    Q_PROPERTY(ApplicationSettings *settings READ settings WRITE setSettings NOTIFY settingsChanged)
    Q_PROPERTY(Swatch *swatch READ swatch CONSTANT)

public:
    enum Type {
        UnknownType,
        TilesetType,
        ImageType,
        LayeredImageType
    };

    Q_ENUM(Type)

    Project();

    virtual Type type() const;
    QString typeString() const;
    static QString typeToString(Project::Type type);
    bool hasLoaded() const;
    bool isNewProject() const;
    void setNewProject(bool newProject);
    bool hasUnsavedChanges() const;
    bool canSave() const;

    QUrl url() const;
    void setUrl(const QUrl &url);
    QString displayUrl() const;
    QUrl dirUrl() const;
    QString fileBaseName() const;

    virtual QSize size() const;
    virtual void setSize(const QSize &size);
    virtual int widthInPixels() const;
    virtual int heightInPixels() const;
    virtual QRect bounds() const;

    QVector<Guide> guides() const;
    virtual void addGuide(const Guide &guide);
    virtual void moveGuide(const Guide &guide, int to);
    virtual void removeGuide(const Guide &guide);

    Swatch *swatch();
    const Swatch *swatch() const;

    virtual int currentLayerIndex() const;

    // Used by animation system (only image projects need to implement this)
    // and MoveContentsDialog.
    Q_INVOKABLE virtual QImage exportedImage() const;

    QUndoStack *undoStack();

    bool isComposingMacro() const;
    void beginMacro(const QString &text);
    void endMacro();
    void addChange(QUndoCommand *undoCommand);
    void clearChanges();

    ApplicationSettings *settings() const;
    void setSettings(ApplicationSettings *settings);

    QJsonObject *cachedProjectJson();

signals:
    void projectCreated();
    void projectLoaded();
    void projectClosed();
    void preProjectSaved();
    void loadedChanged();
    void newProjectChanged();
    void unsavedChangesChanged();
    void canSaveChanged();
    void urlChanged();
    void sizeChanged();
    void errorOccurred(const QString &errorMessage);
    void settingsChanged();
    void guidesChanged();
    void readyForWritingToJson(QJsonObject *projectJson);
    void aboutToBeginMacro(const QString &text);

public slots:
    void load(const QUrl &url);
    void close();
    virtual void save();
    void saveAs(const QUrl &url);
    virtual void revert();

    void importSwatch(const QUrl &swatchUrl);
    void exportSwatch(const QUrl &swatchUrl);

protected:
    void error(const QString &message);

    virtual void doLoad(const QUrl &url);
    virtual void doClose();
    virtual void doSaveAs(const QUrl &url);

    void setComposingMacro(bool composingMacro, const QString &macroText = QString());

    QUrl createTemporaryImage(int width, int height, const QColor &colour);

    void readGuides(const QJsonObject &projectJson);
    void writeGuides(QJsonObject &projectJson) const;

    enum SerialisationFailurePolicy {
        IgnoreSerialisationFailures,
        ErrorOutOnSerialisationFailures
    };

    bool readSwatch(const QJsonObject &projectJson, SerialisationFailurePolicy serialisationFailurePolicy);
    void writeSwatch(QJsonObject &projectJson) const;

    ApplicationSettings *mSettings;

    bool mFromNew;
    QUrl mUrl;
    QTemporaryDir mTempDir;
    bool mUsingTempImage;
    // Caching the project's json object allows the project to save and share
    // e.g. pane info without having to know about the canvas. This member is
    // written to once after loading, and then the readyForWritingToJson()
    // signal is emitted when saving to allow the canvas (and anyone else
    // who may be interested) a chance to save data specific to the project.
    QJsonObject mCachedProjectJson;

    QUndoStack mUndoStack;
    bool mComposingMacro;
    QString mCurrentlyComposingMacroText;
    bool mHadUnsavedChangesBeforeMacroBegan;

    QVector<Guide> mGuides;

    Swatch mSwatch;
};

#endif // PROJECT_H
