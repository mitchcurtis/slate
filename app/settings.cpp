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

#include "settings.h"

#include <QDebug>
#include <QKeySequence>
#include <QLoggingCategory>
#include <QVector>

Q_LOGGING_CATEGORY(lcSettings, "app.settings")

Settings::Settings(QObject *parent) :
    QSettings(parent)
{
    qCDebug(lcSettings) << "Loading settings from" << fileName();
}

QUrl Settings::lastProjectUrl() const
{
    return value("lastProjectUrl").toUrl();
}

void Settings::setLastProjectUrl(const QUrl &url)
{
    QVariant existingValue = value("lastProjectUrl");
    QUrl existingUrl;
    if (contains("lastProjectUrl")) {
        existingUrl = existingValue.toUrl();
    }

    if (url == existingUrl)
        return;

    setValue("lastProjectUrl", url);
    emit lastProjectUrlChanged();
}

bool Settings::loadLastOnStartup() const
{
    return contains("loadLastOnStartup") ? value("loadLastOnStartup").toBool() : defaultLoadLastOnStartup();
}

void Settings::setLoadLastOnStartup(bool loadLastOnStartup)
{
    QVariant existingValue = value("loadLastOnStartup");
    bool existingBoolValue = defaultLoadLastOnStartup();
    if (contains("loadLastOnStartup")) {
        existingBoolValue = existingValue.toBool();
    }

    if (loadLastOnStartup == existingBoolValue)
        return;

    setValue("loadLastOnStartup", loadLastOnStartup);
    emit loadLastOnStartupChanged();
}

bool Settings::defaultLoadLastOnStartup() const
{
    return false;
}

bool Settings::isGridVisible() const
{
    return contains("gridVisible") ? value("gridVisible").toBool() : defaultGridVisible();
}

void Settings::setGridVisible(bool gridVisible)
{
    QVariant existingValue = value("gridVisible");
    bool existingBoolValue = defaultGridVisible();
    if (contains("gridVisible")) {
        existingBoolValue = existingValue.toBool();
    }

    if (gridVisible == existingBoolValue)
        return;

    setValue("gridVisible", gridVisible);
    emit gridVisibleChanged();
}

bool Settings::defaultGridVisible() const
{
    return true;
}

bool Settings::defaultSplitScreen() const
{
    return true;
}

bool Settings::isSplitScreen() const
{
    return contains("splitScreen") ? value("splitScreen").toBool() : defaultSplitScreen();
}

void Settings::setSplitScreen(bool splitScreen)
{
    QVariant existingValue = value("splitScreen");
    bool existingBoolValue = defaultSplitScreen();
    if (contains("splitScreen")) {
        existingBoolValue = existingValue.toBool();
    }

    if (splitScreen == existingBoolValue)
        return;

    setValue("splitScreen", splitScreen);
    emit splitScreenChanged();
}

bool Settings::defaultSplitterLocked() const
{
    return true;
}

bool Settings::isSplitterLocked() const
{
    return contains("splitterLocked") ? value("splitterLocked").toBool() : defaultSplitterLocked();
}

void Settings::setSplitterLocked(bool splitterLocked)
{
    QVariant existingValue = value("splitterLocked");
    bool existingBoolValue = defaultSplitterLocked();
    if (contains("splitterLocked")) {
        existingBoolValue = existingValue.toBool();
    }

    if (splitterLocked == existingBoolValue)
        return;

    setValue("splitterLocked", splitterLocked);
    emit splitterLockedChanged();
}

void Settings::resetShortcutsToDefaults()
{
    static QVector<QString> allShortcuts;
    if (allShortcuts.isEmpty()) {
        allShortcuts.append(QLatin1String("quitShortcut"));
        allShortcuts.append(QLatin1String("newShortcut"));
        allShortcuts.append(QLatin1String("openShortcut"));
        allShortcuts.append(QLatin1String("saveShortcut"));
        allShortcuts.append(QLatin1String("closeShortcut"));
        allShortcuts.append(QLatin1String("revertShortcut"));
        allShortcuts.append(QLatin1String("undoShortcut"));
        allShortcuts.append(QLatin1String("redoShortcut"));
        allShortcuts.append(QLatin1String("gridVisibleShortcut"));
        allShortcuts.append(QLatin1String("splitScreenShortcut"));
        allShortcuts.append(QLatin1String("optionsShortcut"));
        allShortcuts.append(QLatin1String("penToolShortcut"));
        allShortcuts.append(QLatin1String("eyeDropperToolShortcut"));
        allShortcuts.append(QLatin1String("eraserToolShortcut"));
        allShortcuts.append(QLatin1String("toolModeShortcut"));
        allShortcuts.append(QLatin1String("decreaseToolSizeShortcut"));
        allShortcuts.append(QLatin1String("increaseToolSizeShortcut"));
        allShortcuts.append(QLatin1String("swatchLeftShortcut"));
        allShortcuts.append(QLatin1String("swatchRightShortcut"));
        allShortcuts.append(QLatin1String("swatchUpShortcut"));
        allShortcuts.append(QLatin1String("swatchDownShortcut"));
    }

    foreach (const QString &shortcut, allShortcuts) {
        remove(shortcut);
    }
}

#define GET_SHORTCUT(shortcutName, defaultValueFunction) \
    return contains(shortcutName) ? value(shortcutName).toString() : defaultValueFunction();

#define SET_SHORTCUT(shortcutName, defaultValueFunction, notifySignal) \
    QVariant existingValue = value(shortcutName); \
    QString existingStringValue = defaultValueFunction(); \
    if (contains(shortcutName)) { \
        existingStringValue = existingValue.toString(); \
    } \
    \
    if (shortcut == existingStringValue) \
        return; \
    \
    setValue(shortcutName, shortcut); \
    emit notifySignal();

QString Settings::defaultQuitShortcut() const
{
    return QKeySequence(QKeySequence::Quit).toString();
}

QString Settings::quitShortcut() const
{
    GET_SHORTCUT("quitShortcut", defaultQuitShortcut)
}
void Settings::setQuitShortcut(const QString &shortcut)
{
    SET_SHORTCUT("quitShortcut", defaultQuitShortcut, quitShortcutChanged)
}

QString Settings::defaultNewShortcut() const
{
    return QKeySequence(QKeySequence::New).toString();
}

QString Settings::newShortcut() const
{
    GET_SHORTCUT("newShortcut", defaultNewShortcut)
}

void Settings::setNewShortcut(const QString &shortcut)
{
    SET_SHORTCUT("newShortcut", defaultNewShortcut, newShortcutChanged)
}

QString Settings::defaultOpenShortcut() const
{
    return QKeySequence(QKeySequence::Open).toString();
}

QString Settings::openShortcut() const
{
    GET_SHORTCUT("openShortcut", defaultOpenShortcut)
}

void Settings::setOpenShortcut(const QString &shortcut)
{
    SET_SHORTCUT("openShortcut", defaultOpenShortcut, openShortcutChanged)
}

QString Settings::defaultSaveShortcut() const
{
    return QKeySequence(QKeySequence::Save).toString();
}

QString Settings::saveShortcut() const
{
    GET_SHORTCUT("saveShortcut", defaultSaveShortcut)
}

void Settings::setSaveShortcut(const QString &shortcut)
{
    SET_SHORTCUT("saveShortcut", defaultSaveShortcut, saveShortcutChanged)
}

QString Settings::defaultCloseShortcut() const
{
    return QKeySequence(QKeySequence::Close).toString();
}

QString Settings::closeShortcut() const
{
    GET_SHORTCUT("closeShortcut", defaultCloseShortcut)
}

void Settings::setCloseShortcut(const QString &shortcut)
{
    SET_SHORTCUT("closeShortcut", defaultCloseShortcut, closeShortcutChanged)
}

QString Settings::defaultRevertShortcut() const
{
    return QLatin1String("Ctrl+Shift+R");
}

QString Settings::revertShortcut() const
{
    GET_SHORTCUT("revertShortcut", defaultRevertShortcut)
}

void Settings::setRevertShortcut(const QString &shortcut)
{
    SET_SHORTCUT("revertShortcut", defaultRevertShortcut, revertShortcutChanged)
}

QString Settings::defaultUndoShortcut() const
{
    return QKeySequence(QKeySequence::Undo).toString();
}

QString Settings::undoShortcut() const
{
    GET_SHORTCUT("undoShortcut", defaultUndoShortcut)
}

void Settings::setUndoShortcut(const QString &shortcut)
{
    SET_SHORTCUT("undoShortcut", defaultUndoShortcut, undoShortcutChanged)
}

QString Settings::defaultRedoShortcut() const
{
    return QKeySequence(QKeySequence::Redo).toString();
}

QString Settings::redoShortcut() const
{
    GET_SHORTCUT("redoShortcut", defaultRedoShortcut)
}

void Settings::setRedoShortcut(const QString &shortcut)
{
    SET_SHORTCUT("redoShortcut", defaultRedoShortcut, redoShortcutChanged)
}

QString Settings::defaultGridVisibleShortcut() const
{
    return QLatin1String("Ctrl+'");
}

QString Settings::gridVisibleShortcut() const
{
    GET_SHORTCUT("gridVisibleShortcut", defaultGridVisibleShortcut)
}

void Settings::setGridVisibleShortcut(const QString &shortcut)
{
    SET_SHORTCUT("gridVisibleShortcut", defaultGridVisibleShortcut, gridVisibleShortcutChanged)
}

QString Settings::defaultSplitScreenShortcut() const
{
    return QLatin1String("Ctrl+E");
}

QString Settings::splitScreenShortcut() const
{
    GET_SHORTCUT("splitScreenShortcut", defaultSplitScreenShortcut)
}

void Settings::setSplitScreenShortcut(const QString &shortcut)
{
    SET_SHORTCUT("splitScreenShortcut", defaultSplitScreenShortcut, splitScreenShortcutChanged)
}

QString Settings::defaultOptionsShortcut() const
{
    return QLatin1String("Ctrl+Alt+T");
}

QString Settings::optionsShortcut() const
{
    GET_SHORTCUT("optionsShortcut", defaultOptionsShortcut)
}

void Settings::setOptionsShortcut(const QString &shortcut)
{
    SET_SHORTCUT("optionsShortcut", defaultOptionsShortcut, optionsShortcutChanged)
}

QString Settings::defaultPenToolShortcut() const
{
    return QLatin1String("B");
}

QString Settings::penToolShortcut() const
{
    GET_SHORTCUT("penToolShortcut", defaultPenToolShortcut)
}

void Settings::setPenToolShortcut(const QString &shortcut)
{
    SET_SHORTCUT("penToolShortcut", defaultPenToolShortcut, penToolShortcutChanged)
}

QString Settings::defaultEyeDropperToolShortcut() const
{
    return QLatin1String("I");
}

QString Settings::eyeDropperToolShortcut() const
{
    GET_SHORTCUT("eyeDropperToolShortcut", defaultEyeDropperToolShortcut)
}

void Settings::setEyeDropperToolShortcut(const QString &shortcut)
{
    SET_SHORTCUT("eyeDropperToolShortcut", defaultEyeDropperToolShortcut, eyeDropperToolShortcutChanged)
}

QString Settings::defaultEraserToolShortcut() const
{
    return QLatin1String("E");
}

QString Settings::eraserToolShortcut() const
{
    GET_SHORTCUT("eraserToolShortcut", defaultEraserToolShortcut)
}

void Settings::setEraserToolShortcut(const QString &shortcut)
{
    SET_SHORTCUT("eraserToolShortcut", defaultEraserToolShortcut, eraserToolShortcutChanged)
}

QString Settings::defaultToolModeShortcut() const
{
    return QLatin1String("T");
}

QString Settings::toolModeShortcut() const
{
    GET_SHORTCUT("toolModeShortcut", defaultToolModeShortcut)
}

void Settings::setToolModeShortcut(const QString &shortcut)
{
    SET_SHORTCUT("toolModeShortcut", defaultToolModeShortcut, toolModeShortcutChanged)
}

QString Settings::defaultDecreaseToolSizeShortcut() const
{
    return QLatin1String("[");
}

QString Settings::decreaseToolSizeShortcut() const
{
    GET_SHORTCUT("decreaseToolSizeShortcut", defaultDecreaseToolSizeShortcut)
}

void Settings::setDecreaseToolSizeShortcut(const QString &shortcut)
{
    SET_SHORTCUT("decreaseToolSizeShortcut", defaultDecreaseToolSizeShortcut, decreaseToolSizeShortcutChanged)
}

QString Settings::defaultIncreaseToolSizeShortcut() const
{
    return QLatin1String("]");
}

QString Settings::increaseToolSizeShortcut() const
{
    GET_SHORTCUT("increaseToolSizeShortcut", defaultIncreaseToolSizeShortcut)
}

void Settings::setIncreaseToolSizeShortcut(const QString &shortcut)
{
    SET_SHORTCUT("increaseToolSizeShortcut", defaultIncreaseToolSizeShortcut, increaseToolSizeShortcutChanged)
}

QString Settings::defaultSwatchLeftShortcut() const
{
    return QLatin1String("A");
}

QString Settings::swatchLeftShortcut() const
{
    GET_SHORTCUT("swatchLeftShortcut", defaultSwatchLeftShortcut)
}

void Settings::setSwatchLeftShortcut(const QString &shortcut)
{
    SET_SHORTCUT("swatchLeftShortcut", defaultSwatchLeftShortcut, swatchLeftShortcutChanged)
}

QString Settings::defaultSwatchRightShortcut() const
{
    return QLatin1String("D");
}

QString Settings::swatchRightShortcut() const
{
    GET_SHORTCUT("swatchRightShortcut", defaultSwatchRightShortcut)
}

void Settings::setSwatchRightShortcut(const QString &shortcut)
{
    SET_SHORTCUT("swatchRightShortcut", defaultSwatchRightShortcut, swatchRightShortcutChanged)
}

QString Settings::defaultSwatchUpShortcut() const
{
    return QLatin1String("W");
}

QString Settings::swatchUpShortcut() const
{
    GET_SHORTCUT("swatchUpShortcut", defaultSwatchUpShortcut)
}

void Settings::setSwatchUpShortcut(const QString &shortcut)
{
    SET_SHORTCUT("swatchUpShortcut", defaultSwatchUpShortcut, swatchUpShortcutChanged)
}

QString Settings::defaultSwatchDownShortcut() const
{
    return QLatin1String("S");
}

QString Settings::swatchDownShortcut() const
{
    GET_SHORTCUT("swatchDownShortcut", defaultSwatchDownShortcut)
}

void Settings::setSwatchDownShortcut(const QString &shortcut)
{
    SET_SHORTCUT("swatchDownShortcut", defaultSwatchDownShortcut, swatchDownShortcutChanged)
}
