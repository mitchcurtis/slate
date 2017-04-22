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

Q_DECLARE_LOGGING_CATEGORY(lcProject)
Q_DECLARE_LOGGING_CATEGORY(lcProjectLifecycle)

class Project : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool loaded READ hasLoaded NOTIFY loadedChanged)
    Q_PROPERTY(bool newProject READ isNewProject WRITE setNewProject NOTIFY newProjectChanged)
    Q_PROPERTY(bool unsavedChanges READ hasUnsavedChanges NOTIFY unsavedChangesChanged)
    Q_PROPERTY(bool canSave READ canSave NOTIFY canSaveChanged)
    Q_PROPERTY(QUrl url READ url NOTIFY urlChanged)
    Q_PROPERTY(QSize size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(UndoStack *undoStack READ undoStack CONSTANT)

public:
    Project();

    bool hasLoaded() const;
    bool isNewProject() const;
    void setNewProject(bool newProject);
    bool hasUnsavedChanges() const;
    bool canSave() const;

    QUrl url() const;
    void setUrl(const QUrl &url);

    virtual QSize size() const;
    virtual void setSize(const QSize &size);

    virtual int widthInPixels() const;
    virtual int heightInPixels() const;

    UndoStack *undoStack();

    bool isComposingMacro() const;
    void beginMacro(const QString &text);
    void endMacro();
    void addChange(UndoCommand *undoCommand);
    void clearChanges();

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

    bool mFromNew;
    QUrl mUrl;
    QTemporaryDir mTempDir;
    bool mUsingTempImage;

    UndoStack mUndoStack;
    bool mComposingMacro;
    bool mHadUnsavedChangesBeforeMacroBegan;
};

#endif // PROJECT_H
