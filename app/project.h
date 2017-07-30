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

#include <QLoggingCategory>
#include <QObject>
#include <QSize>
#include <QTemporaryDir>
#include <QUrl>

#include <QtUndo/undostack.h>

#include "guide.h"

Q_DECLARE_LOGGING_CATEGORY(lcProject)
Q_DECLARE_LOGGING_CATEGORY(lcProjectLifecycle)

class ApplicationSettings;

class Project : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Type type READ type CONSTANT)
    Q_PROPERTY(QString typeString READ typeString CONSTANT)
    Q_PROPERTY(bool loaded READ hasLoaded NOTIFY loadedChanged)
    Q_PROPERTY(bool newProject READ isNewProject WRITE setNewProject NOTIFY newProjectChanged)
    Q_PROPERTY(bool unsavedChanges READ hasUnsavedChanges NOTIFY unsavedChangesChanged)
    Q_PROPERTY(bool canSave READ canSave NOTIFY canSaveChanged)
    Q_PROPERTY(QUrl url READ url NOTIFY urlChanged)
    Q_PROPERTY(QString displayUrl READ displayUrl NOTIFY urlChanged)
    Q_PROPERTY(QSize size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(UndoStack *undoStack READ undoStack CONSTANT)
    Q_PROPERTY(ApplicationSettings *settings READ settings WRITE setSettings NOTIFY settingsChanged)

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

    virtual QSize size() const;
    virtual void setSize(const QSize &size);

    virtual int widthInPixels() const;
    virtual int heightInPixels() const;

    QVector<Guide> guides() const;
    virtual void addGuide(const Guide &guide);
    virtual void moveGuide(const Guide &guide, int to);
    virtual void removeGuide(const Guide &guide);

    UndoStack *undoStack();

    bool isComposingMacro() const;
    void beginMacro(const QString &text);
    void endMacro();
    void addChange(UndoCommand *undoCommand);
    void clearChanges();

    ApplicationSettings *settings() const;
    void setSettings(ApplicationSettings *settings);

signals:
    void projectCreated();
    void projectLoaded();
    void projectClosed();
    void loadedChanged();
    void newProjectChanged();
    void unsavedChangesChanged();
    void canSaveChanged();
    void urlChanged();
    void sizeChanged();
    void errorOccurred(const QString &errorMessage);
    void settingsChanged();
    void guidesChanged();

public slots:
    virtual void load(const QUrl &url);
    virtual void close();
    virtual void save();
    virtual void saveAs(const QUrl &url);
    virtual void revert();

protected:
    void error(const QString &message);

    void setComposingMacro(bool composingMacro);

    QUrl createTemporaryImage(int width, int height, const QColor &colour);

    void readGuides(const QJsonObject &projectJson);
    void writeGuides(QJsonObject &projectJson) const;

    ApplicationSettings *mSettings;

    bool mFromNew;
    QUrl mUrl;
    QTemporaryDir mTempDir;
    bool mUsingTempImage;

    UndoStack mUndoStack;
    bool mComposingMacro;
    bool mHadUnsavedChangesBeforeMacroBegan;

    QVector<Guide> mGuides;
};

#endif // PROJECT_H
