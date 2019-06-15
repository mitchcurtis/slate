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

#include "applicationsettings.h"

#include <QDebug>
#include <QFile>
#include <QKeySequence>
#include <QLoggingCategory>
#include <QVector>

Q_LOGGING_CATEGORY(lcApplicationSettings, "app.applicationsettings")

ApplicationSettings::ApplicationSettings(QObject *parent) :
    QSettings(parent)
{
    qCDebug(lcApplicationSettings) << "Loading settings from" << fileName();
}

QString ApplicationSettings::defaultLanguage() const
{
    return "en_GB";
}

QString ApplicationSettings::language() const
{
     return contains("language") ? value("language").toString() : defaultLanguage();
}

void ApplicationSettings::setLanguage(const QString &language)
{
    const QVariant existingValue = value("language");
    QString existingStringValue = defaultLanguage();
    if (contains("language"))
        existingStringValue = existingValue.toBool();

    if (language == existingStringValue)
        return;

    setValue("language", language);
    emit languageChanged();
}

bool ApplicationSettings::loadLastOnStartup() const
{
    return contains("loadLastOnStartup") ? value("loadLastOnStartup").toBool() : defaultLoadLastOnStartup();
}

void ApplicationSettings::setLoadLastOnStartup(bool loadLastOnStartup)
{
    // TODO: replace all this duplicated code with the getter
    // e.g. if (this->loadLastOnStartup() == loadLastOnStartup) return;
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

bool ApplicationSettings::defaultLoadLastOnStartup() const
{
    return false;
}

QStringList ApplicationSettings::recentFiles() const
{
    return contains("recentFiles") ? value("recentFiles").toStringList() : QStringList();
}

void ApplicationSettings::addRecentFile(const QString &filePath)
{
    QStringList files = recentFiles();
    const int existingIndex = files.indexOf(filePath);
    // If it already exists, remove it and move it to the top.
    if (existingIndex != -1)
        files.removeAt(existingIndex);

    // Add the file to the top of the list.
    files.prepend(filePath);

    // Respect the file limit.
    if (files.size() > 20)
        files.removeLast();

    setValue("recentFiles", files);
    emit recentFilesChanged();
}

void ApplicationSettings::clearRecentFiles()
{
    if (recentFiles().isEmpty())
        return;

    setValue("recentFiles", QStringList());
    emit recentFilesChanged();
}

void ApplicationSettings::removeInvalidRecentFiles()
{
    if (!contains("recentFiles"))
        return;

    bool changed = false;
    QStringList files = value("recentFiles").toStringList();
    for (int i = 0; i < files.size(); ) {
        const QString filePath = files.at(i);
        if (filePath.isEmpty() || !QFile::exists(QUrl(filePath).toLocalFile())) {
            files.removeAt(i);
            changed = true;
        } else {
            ++i;
        }
    }

    setValue("recentFiles", files);

    if (changed)
        emit recentFilesChanged();
}

QString ApplicationSettings::displayableFilePath(const QString &filePath) const
{
    return QUrl(filePath).path();
}

bool ApplicationSettings::isGridVisible() const
{
    return contains("gridVisible") ? value("gridVisible").toBool() : defaultGridVisible();
}

void ApplicationSettings::setGridVisible(bool gridVisible)
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

bool ApplicationSettings::defaultGridVisible() const
{
    return true;
}

bool ApplicationSettings::areRulersVisible() const
{
    return contains("rulersVisible") ? value("rulersVisible").toBool() : defaultRulersVisible();
}

void ApplicationSettings::setRulersVisible(bool rulersVisible)
{
    QVariant existingValue = value("rulersVisible");
    bool existingBoolValue = defaultRulersVisible();
    if (contains("rulersVisible")) {
        existingBoolValue = existingValue.toBool();
    }

    if (rulersVisible == existingBoolValue)
        return;

    setValue("rulersVisible", rulersVisible);
    emit rulersVisibleChanged();
}

bool ApplicationSettings::defaultRulersVisible() const
{
    return true;
}

bool ApplicationSettings::areGuidesVisible() const
{
    return contains("guidesVisible") ? value("guidesVisible").toBool() : defaultGuidesVisible();
}

void ApplicationSettings::setGuidesVisible(bool guidesVisible)
{
    QVariant existingValue = value("guidesVisible");
    bool existingBoolValue = defaultGuidesVisible();
    if (contains("guidesVisible")) {
        existingBoolValue = existingValue.toBool();
    }

    if (guidesVisible == existingBoolValue)
        return;

    setValue("guidesVisible", guidesVisible);
    emit guidesVisibleChanged();
}

bool ApplicationSettings::defaultGuidesVisible() const
{
    return true;
}

bool ApplicationSettings::areGuidesLocked() const
{
    return contains("guidesLocked") ? value("guidesLocked").toBool() : defaultGuidesLocked();
}

void ApplicationSettings::setGuidesLocked(bool guidesLocked)
{
    QVariant existingValue = value("guidesLocked");
    bool existingBoolValue = defaultGuidesLocked();
    if (contains("guidesLocked")) {
        existingBoolValue = existingValue.toBool();
    }

    if (guidesLocked == existingBoolValue)
        return;

    setValue("guidesLocked", guidesLocked);
    emit guidesLockedChanged();
}

bool ApplicationSettings::defaultGuidesLocked() const
{
    return false;
}

bool ApplicationSettings::defaultScrollZoom() const
{
    return true;
}

bool ApplicationSettings::scrollZoom() const
{
    return contains("scrollZoom") ? value("scrollZoom").toBool() : defaultScrollZoom();
}

void ApplicationSettings::setScrollZoom(bool scrollZoom)
{
    QVariant existingValue = value("scrollZoom");
    bool existingBoolValue = defaultScrollZoom();
    if (contains("scrollZoom")) {
        existingBoolValue = existingValue.toBool();
    }

    if (scrollZoom == existingBoolValue)
        return;

    setValue("scrollZoom", scrollZoom);
    emit scrollZoomChanged();
}

bool ApplicationSettings::defaultFpsVisible() const
{
    return false;
}

bool ApplicationSettings::isFpsVisible() const
{
    return contains("fpsVisible") ? value("fpsVisible").toBool() : defaultFpsVisible();
}

void ApplicationSettings::setFpsVisible(bool fpsVisible)
{
    QVariant existingValue = value("fpsVisible");
    bool existingBoolValue = defaultFpsVisible();
    if (contains("fpsVisible")) {
        existingBoolValue = existingValue.toBool();
    }

    if (fpsVisible == existingBoolValue)
        return;

    setValue("fpsVisible", fpsVisible);
    emit fpsVisibleChanged();
}

bool ApplicationSettings::defaultGesturesEnabled() const
{
#ifdef Q_OS_MACOS
    return true;
#else
    return false;
#endif
}

bool ApplicationSettings::areGesturesEnabled() const
{
    return contains("gesturesEnabled") ? value("gesturesEnabled").toBool() : defaultGesturesEnabled();
}

void ApplicationSettings::setGesturesEnabled(bool gesturesEnabled)
{
    const QVariant existingValue = value("gesturesEnabled");
    bool existingBoolValue = defaultGesturesEnabled();
    if (contains("gesturesEnabled")) {
        existingBoolValue = existingValue.toBool();
    }

    if (gesturesEnabled == existingBoolValue)
        return;

    setValue("gesturesEnabled", gesturesEnabled);
    emit gesturesEnabledChanged();
}

bool ApplicationSettings::defaultAutoSwatchEnabled() const
{
    return false;
}

bool ApplicationSettings::isAutoSwatchEnabled() const
{
    return contains("autoSwatchEnabled") ? value("autoSwatchEnabled").toBool() : defaultAutoSwatchEnabled();
}

void ApplicationSettings::setAutoSwatchEnabled(bool autoSwatchEnabled)
{
    const QVariant existingValue = value("autoSwatchEnabled");
    bool existingBoolValue = defaultAutoSwatchEnabled();
    if (contains("autoSwatchEnabled")) {
        existingBoolValue = existingValue.toBool();
    }

    if (autoSwatchEnabled == existingBoolValue)
        return;

    setValue("autoSwatchEnabled", autoSwatchEnabled);
    emit autoSwatchEnabledChanged();
}

bool ApplicationSettings::defaultAlwaysShowCrosshair() const
{
    return false;
}

bool ApplicationSettings::isAlwaysShowCrosshair() const
{
    return contains("alwaysShowCrosshair") ? value("alwaysShowCrosshair").toBool() : defaultAlwaysShowCrosshair();
}

void ApplicationSettings::setAlwaysShowCrosshair(bool alwaysShowCrosshair)
{
    const QVariant existingValue = value("alwaysShowCrosshair");
    bool existingBoolValue = defaultAlwaysShowCrosshair();
    if (contains("alwaysShowCrosshair")) {
        existingBoolValue = existingValue.toBool();
    }

    if (alwaysShowCrosshair == existingBoolValue)
        return;

    setValue("alwaysShowCrosshair", alwaysShowCrosshair);
    emit alwaysShowCrosshairChanged();
}

qreal ApplicationSettings::defaultWindowOpacity() const
{
    return 1.0;
}

qreal ApplicationSettings::windowOpacity() const
{
    return contains("windowOpacity") ? value("windowOpacity").toReal() : defaultWindowOpacity();
}

void ApplicationSettings::setWindowOpacity(qreal opacity)
{
    if (windowOpacity() == opacity)
        return;

    setValue("windowOpacity", opacity);
    emit windowOpacityChanged();
}

QColor ApplicationSettings::defaultCheckerColour1() const
{
    return QColor::fromRgb(0x444444);
}

QColor ApplicationSettings::checkerColour1() const
{
    return contains("checkerColour1") ? value("checkerColour1").value<QColor>() : defaultCheckerColour1();
}

void ApplicationSettings::setCheckerColour1(const QColor &colour)
{
    if (colour == checkerColour1())
        return;

    setValue("checkerColour1", QVariant::fromValue(colour));
    emit checkerColour1Changed();
}

QColor ApplicationSettings::defaultCheckerColour2() const
{
    return QColor::fromRgb(0x7e7e7e);
}

QColor ApplicationSettings::checkerColour2() const
{
    return contains("checkerColour2") ? value("checkerColour2").value<QColor>() : defaultCheckerColour2();
}

void ApplicationSettings::setCheckerColour2(const QColor &colour)
{
    if (colour == checkerColour2())
        return;

    setValue("checkerColour2", QVariant::fromValue(colour));
    emit checkerColour2Changed();
}

int ApplicationSettings::defaultPenToolRightClickBehaviour() const
{
    return 0; // RightClickAppliesEraser
}

int ApplicationSettings::penToolRightClickBehaviour() const
{
    return contains("penToolRightClickBehaviour")
        ? value("penToolRightClickBehaviour").value<int>() : defaultPenToolRightClickBehaviour();
}

void ApplicationSettings::setPenToolRightClickBehaviour(int penToolRightClickBehaviour)
{
    if (this->penToolRightClickBehaviour() == penToolRightClickBehaviour)
        return;

    setValue("penToolRightClickBehaviour", QVariant(penToolRightClickBehaviour));
    emit penToolRightClickBehaviourChanged();
}

void ApplicationSettings::resetShortcutsToDefaults()
{
    static QVector<QString> allShortcuts;
    if (allShortcuts.isEmpty()) {
        allShortcuts.append(QLatin1String("quitShortcut"));
        allShortcuts.append(QLatin1String("newShortcut"));
        allShortcuts.append(QLatin1String("openShortcut"));
        allShortcuts.append(QLatin1String("saveShortcut"));
        allShortcuts.append(QLatin1String("saveAsShortcut"));
        allShortcuts.append(QLatin1String("exportShortcut"));
        allShortcuts.append(QLatin1String("closeShortcut"));
        allShortcuts.append(QLatin1String("revertShortcut"));
        allShortcuts.append(QLatin1String("undoShortcut"));
        allShortcuts.append(QLatin1String("redoShortcut"));
        allShortcuts.append(QLatin1String("flipHorizontallyShortcut"));
        allShortcuts.append(QLatin1String("flipVerticallyShortcut"));
        allShortcuts.append(QLatin1String("resizeCanvasShortcut"));
        allShortcuts.append(QLatin1String("resizeImageShortcut"));
        allShortcuts.append(QLatin1String("moveContentsShortcut"));
        allShortcuts.append(QLatin1String("centreShortcut"));
        allShortcuts.append(QLatin1String("zoomInShortcut"));
        allShortcuts.append(QLatin1String("zoomOutShortcut"));
        allShortcuts.append(QLatin1String("gridVisibleShortcut"));
        allShortcuts.append(QLatin1String("rulersVisibleShortcut"));
        allShortcuts.append(QLatin1String("guidesVisibleShortcut"));
        allShortcuts.append(QLatin1String("splitScreenShortcut"));
        allShortcuts.append(QLatin1String("splitterLockedShortcut"));
        allShortcuts.append(QLatin1String("optionsShortcut"));
        allShortcuts.append(QLatin1String("penToolShortcut"));
        allShortcuts.append(QLatin1String("eyeDropperToolShortcut"));
        allShortcuts.append(QLatin1String("fillToolShortcut"));
        allShortcuts.append(QLatin1String("eraserToolShortcut"));
        allShortcuts.append(QLatin1String("selectionToolShortcut"));
        allShortcuts.append(QLatin1String("toolModeShortcut"));
        allShortcuts.append(QLatin1String("decreaseToolSizeShortcut"));
        allShortcuts.append(QLatin1String("increaseToolSizeShortcut"));
        allShortcuts.append(QLatin1String("swatchLeftShortcut"));
        allShortcuts.append(QLatin1String("swatchRightShortcut"));
        allShortcuts.append(QLatin1String("swatchUpShortcut"));
        allShortcuts.append(QLatin1String("swatchDownShortcut"));
        allShortcuts.append(QLatin1String("fullScreenToggleShortcut"));
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

QString ApplicationSettings::defaultQuitShortcut() const
{
    return QKeySequence(QKeySequence::Quit).toString();
}

QString ApplicationSettings::quitShortcut() const
{
    GET_SHORTCUT("quitShortcut", defaultQuitShortcut)
}
void ApplicationSettings::setQuitShortcut(const QString &shortcut)
{
    SET_SHORTCUT("quitShortcut", defaultQuitShortcut, quitShortcutChanged)
}

QString ApplicationSettings::defaultNewShortcut() const
{
    return QKeySequence(QKeySequence::New).toString();
}

QString ApplicationSettings::newShortcut() const
{
    GET_SHORTCUT("newShortcut", defaultNewShortcut)
}

void ApplicationSettings::setNewShortcut(const QString &shortcut)
{
    SET_SHORTCUT("newShortcut", defaultNewShortcut, newShortcutChanged)
}

QString ApplicationSettings::defaultOpenShortcut() const
{
    return QKeySequence(QKeySequence::Open).toString();
}

QString ApplicationSettings::openShortcut() const
{
    GET_SHORTCUT("openShortcut", defaultOpenShortcut)
}

void ApplicationSettings::setOpenShortcut(const QString &shortcut)
{
    SET_SHORTCUT("openShortcut", defaultOpenShortcut, openShortcutChanged)
}

QString ApplicationSettings::defaultSaveShortcut() const
{
    return QKeySequence(QKeySequence::Save).toString();
}

QString ApplicationSettings::saveShortcut() const
{
    GET_SHORTCUT("saveShortcut", defaultSaveShortcut)
}

void ApplicationSettings::setSaveShortcut(const QString &shortcut)
{
    SET_SHORTCUT("saveShortcut", defaultSaveShortcut, saveShortcutChanged)
}

QString ApplicationSettings::defaultSaveAsShortcut() const
{
    return QKeySequence(QKeySequence::SaveAs).toString();
}

QString ApplicationSettings::saveAsShortcut() const
{
    GET_SHORTCUT("saveAsShortcut", defaultSaveAsShortcut)
}

void ApplicationSettings::setSaveAsShortcut(const QString &shortcut)
{
    SET_SHORTCUT("saveAsShortcut", defaultSaveAsShortcut, saveAsShortcutChanged)
}

QString ApplicationSettings::defaultExportShortcut() const
{
    return QLatin1String("Ctrl+Shift+E");
}

QString ApplicationSettings::exportShortcut() const
{
    GET_SHORTCUT("exportShortcut", defaultExportShortcut)
}

void ApplicationSettings::setExportShortcut(const QString &shortcut)
{
    SET_SHORTCUT("exportShortcut", defaultExportShortcut, exportShortcutChanged)
}

QString ApplicationSettings::defaultCloseShortcut() const
{
#ifdef Q_OS_WIN
    // Ctrl+F4 is apparently the default, but is not what we want.
    return QLatin1String("Ctrl+W");
#else
    return QKeySequence(QKeySequence::Close).toString();
#endif
}

QString ApplicationSettings::closeShortcut() const
{
    GET_SHORTCUT("closeShortcut", defaultCloseShortcut)
}

void ApplicationSettings::setCloseShortcut(const QString &shortcut)
{
    SET_SHORTCUT("closeShortcut", defaultCloseShortcut, closeShortcutChanged)
}

QString ApplicationSettings::defaultRevertShortcut() const
{
    return QLatin1String("Ctrl+Shift+R");
}

QString ApplicationSettings::revertShortcut() const
{
    GET_SHORTCUT("revertShortcut", defaultRevertShortcut)
}

void ApplicationSettings::setRevertShortcut(const QString &shortcut)
{
    SET_SHORTCUT("revertShortcut", defaultRevertShortcut, revertShortcutChanged)
}

QString ApplicationSettings::defaultUndoShortcut() const
{
    return QKeySequence(QKeySequence::Undo).toString();
}

QString ApplicationSettings::undoShortcut() const
{
    GET_SHORTCUT("undoShortcut", defaultUndoShortcut)
}

void ApplicationSettings::setUndoShortcut(const QString &shortcut)
{
    SET_SHORTCUT("undoShortcut", defaultUndoShortcut, undoShortcutChanged)
}

QString ApplicationSettings::defaultRedoShortcut() const
{
    return QKeySequence(QKeySequence::Redo).toString();
}

QString ApplicationSettings::redoShortcut() const
{
    GET_SHORTCUT("redoShortcut", defaultRedoShortcut)
}

void ApplicationSettings::setRedoShortcut(const QString &shortcut)
{
    SET_SHORTCUT("redoShortcut", defaultRedoShortcut, redoShortcutChanged)
}

QString ApplicationSettings::defaultFlipHorizontallyShortcut() const
{
    return QLatin1String("H");
}

QString ApplicationSettings::flipHorizontallyShortcut() const
{
    GET_SHORTCUT("flipHorizontallyShortcut", defaultFlipHorizontallyShortcut)
}

void ApplicationSettings::setFlipHorizontallyShortcut(const QString &shortcut)
{
    SET_SHORTCUT("flipHorizontallyShortcut", defaultFlipHorizontallyShortcut, flipHorizontallyShortcutChanged)
}

QString ApplicationSettings::defaultFlipVerticallyShortcut() const
{
    return QLatin1String("V");
}

QString ApplicationSettings::flipVerticallyShortcut() const
{
    GET_SHORTCUT("flipVerticallyShortcut", defaultFlipVerticallyShortcut)
}

void ApplicationSettings::setFlipVerticallyShortcut(const QString &shortcut)
{
    SET_SHORTCUT("flipVerticallyShortcut", defaultFlipVerticallyShortcut, flipVerticallyShortcutChanged)
}

QString ApplicationSettings::defaultResizeCanvasShortcut() const
{
    return QLatin1String("Ctrl+Shift+C");
}

QString ApplicationSettings::resizeCanvasShortcut() const
{
    GET_SHORTCUT("resizeCanvasShortcut", defaultResizeCanvasShortcut)
}

void ApplicationSettings::setResizeCanvasShortcut(const QString &shortcut)
{
    SET_SHORTCUT("resizeCanvasShortcut", defaultResizeCanvasShortcut, resizeCanvasShortcutChanged)
}

QString ApplicationSettings::defaultResizeImageShortcut() const
{
    return QLatin1String("Ctrl+Shift+I");
}

QString ApplicationSettings::resizeImageShortcut() const
{
    GET_SHORTCUT("resizeImageShortcut", defaultResizeImageShortcut)
}

void ApplicationSettings::setResizeImageShortcut(const QString &shortcut)
{
    SET_SHORTCUT("resizeImageShortcut", defaultResizeImageShortcut, resizeImageShortcutChanged)
}

QString ApplicationSettings::defaultMoveContentsShortcut() const
{
    return QLatin1String("Alt+Shift+C");
}

QString ApplicationSettings::moveContentsShortcut() const
{
    GET_SHORTCUT("moveContentsShortcut", defaultMoveContentsShortcut)
}

void ApplicationSettings::setMoveContentsShortcut(const QString &shortcut)
{
    SET_SHORTCUT("moveContentsShortcut", defaultMoveContentsShortcut, moveContentsShortcutChanged)
}

QString ApplicationSettings::defaultGridVisibleShortcut() const
{
    return QLatin1String("Ctrl+'");
}

QString ApplicationSettings::gridVisibleShortcut() const
{
    GET_SHORTCUT("gridVisibleShortcut", defaultGridVisibleShortcut)
}

void ApplicationSettings::setGridVisibleShortcut(const QString &shortcut)
{
    SET_SHORTCUT("gridVisibleShortcut", defaultGridVisibleShortcut, gridVisibleShortcutChanged)
}

QString ApplicationSettings::defaultRulersVisibleShortcut() const
{
    return QLatin1String("Ctrl+R");
}

QString ApplicationSettings::rulersVisibleShortcut() const
{
    GET_SHORTCUT("rulersVisibleShortcut", defaultRulersVisibleShortcut)
}

void ApplicationSettings::setRulersVisibleShortcut(const QString &shortcut)
{
    SET_SHORTCUT("rulersVisibleShortcut", defaultRulersVisibleShortcut, rulersVisibleShortcutChanged)
}

QString ApplicationSettings::defaultGuidesVisibleShortcut() const
{
    return QLatin1String("Ctrl+;");
}

QString ApplicationSettings::guidesVisibleShortcut() const
{
    GET_SHORTCUT("guidesVisibleShortcut", defaultGuidesVisibleShortcut)
}

void ApplicationSettings::setGuidesVisibleShortcut(const QString &shortcut)
{
    SET_SHORTCUT("guidesVisibleShortcut", defaultGuidesVisibleShortcut, guidesVisibleShortcutChanged)
}

QString ApplicationSettings::defaultCentreShortcut() const
{
#ifdef Q_OS_MACOS
    // Cmd+Space is taken by Spotlight.
    return QLatin1String("Meta+Space");
#else
    return QLatin1String("Ctrl+Space");
#endif
}

QString ApplicationSettings::centreShortcut() const
{
    GET_SHORTCUT("centreShortcut", defaultCentreShortcut)
}

void ApplicationSettings::setCentreShortcut(const QString &shortcut)
{
    SET_SHORTCUT("centreShortcut", defaultCentreShortcut, centreShortcutChanged)
}

QString ApplicationSettings::defaultZoomInShortcut() const
{
    return QKeySequence(QKeySequence::ZoomIn).toString();
}

QString ApplicationSettings::zoomInShortcut() const
{
    GET_SHORTCUT("zoomInShortcut", defaultZoomInShortcut)
}

void ApplicationSettings::setZoomInShortcut(const QString &shortcut)
{
    SET_SHORTCUT("zoomInShortcut", defaultZoomInShortcut, zoomInShortcutChanged)
}

QString ApplicationSettings::defaultZoomOutShortcut() const
{
    return QKeySequence(QKeySequence::ZoomOut).toString();
}

QString ApplicationSettings::zoomOutShortcut() const
{
    GET_SHORTCUT("zoomOutShortcut", defaultZoomOutShortcut)
}

void ApplicationSettings::setZoomOutShortcut(const QString &shortcut)
{
    SET_SHORTCUT("zoomOutShortcut", defaultZoomOutShortcut, zoomOutShortcutChanged)
}

QString ApplicationSettings::defaultSplitScreenShortcut() const
{
    return QLatin1String("Ctrl+E");
}

QString ApplicationSettings::splitScreenShortcut() const
{
    GET_SHORTCUT("splitScreenShortcut", defaultSplitScreenShortcut)
}

void ApplicationSettings::setSplitScreenShortcut(const QString &shortcut)
{
    SET_SHORTCUT("splitScreenShortcut", defaultSplitScreenShortcut, splitScreenShortcutChanged)
}

QString ApplicationSettings::defaultSplitterLockedShortcut() const
{
    return QLatin1String("Ctrl+Alt+E");
}

QString ApplicationSettings::splitterLockedShortcut() const
{
    GET_SHORTCUT("splitterLockedShortcut", defaultSplitterLockedShortcut)
}

void ApplicationSettings::setSplitterLockedShortcut(const QString &shortcut)
{
    SET_SHORTCUT("splitterLockedShortcut", defaultSplitterLockedShortcut, splitterLockedShortcutChanged)
}

QString ApplicationSettings::defaultAnimationPlaybackShortcut() const
{
    return QLatin1String("A");
}

QString ApplicationSettings::animationPlaybackShortcut() const
{
    GET_SHORTCUT("animationPlaybackShortcut", defaultAnimationPlaybackShortcut)
}

void ApplicationSettings::setAnimationPlaybackShortcut(const QString &shortcut)
{
    SET_SHORTCUT("animationPlaybackShortcut", defaultAnimationPlaybackShortcut, animationPlaybackShortcutChanged)
}

QString ApplicationSettings::defaultOptionsShortcut() const
{
#if defined(Q_OS_MACOS)
    return QKeySequence(QKeySequence::Preferences).toString();
#else
    // According to the documentation, QKeySequence::Preferences is only defined for macOS,
    // so we define it for the rest of the platforms here.
    return QLatin1String("Ctrl+Alt+T");
#endif
}

QString ApplicationSettings::optionsShortcut() const
{
    GET_SHORTCUT("optionsShortcut", defaultOptionsShortcut)
}

void ApplicationSettings::setOptionsShortcut(const QString &shortcut)
{
    SET_SHORTCUT("optionsShortcut", defaultOptionsShortcut, optionsShortcutChanged)
}

QString ApplicationSettings::defaultPenToolShortcut() const
{
    return QLatin1String("B");
}

QString ApplicationSettings::penToolShortcut() const
{
    GET_SHORTCUT("penToolShortcut", defaultPenToolShortcut)
}

void ApplicationSettings::setPenToolShortcut(const QString &shortcut)
{
    SET_SHORTCUT("penToolShortcut", defaultPenToolShortcut, penToolShortcutChanged)
}

QString ApplicationSettings::defaultEyeDropperToolShortcut() const
{
    return QLatin1String("I");
}

QString ApplicationSettings::eyeDropperToolShortcut() const
{
    GET_SHORTCUT("eyeDropperToolShortcut", defaultEyeDropperToolShortcut)
}

void ApplicationSettings::setEyeDropperToolShortcut(const QString &shortcut)
{
    SET_SHORTCUT("eyeDropperToolShortcut", defaultEyeDropperToolShortcut, eyeDropperToolShortcutChanged)
}

QString ApplicationSettings::defaultFillToolShortcut() const
{
    return QLatin1String("G");
}

QString ApplicationSettings::fillToolShortcut() const
{
    GET_SHORTCUT("fillToolShortcut", defaultFillToolShortcut)
}

void ApplicationSettings::setFillToolShortcut(const QString &shortcut)
{
    SET_SHORTCUT("fillToolShortcut", defaultFillToolShortcut, fillToolShortcutChanged)
}

QString ApplicationSettings::defaultEraserToolShortcut() const
{
    return QLatin1String("E");
}

QString ApplicationSettings::eraserToolShortcut() const
{
    GET_SHORTCUT("eraserToolShortcut", defaultEraserToolShortcut)
}

void ApplicationSettings::setEraserToolShortcut(const QString &shortcut)
{
    SET_SHORTCUT("eraserToolShortcut", defaultEraserToolShortcut, eraserToolShortcutChanged)
}

QString ApplicationSettings::defaultSelectionToolShortcut() const
{
    return QLatin1String("M");
}

QString ApplicationSettings::selectionToolShortcut() const
{
    GET_SHORTCUT("selectionToolShortcut", defaultSelectionToolShortcut)
}

void ApplicationSettings::setSelectionToolShortcut(const QString &shortcut)
{
    SET_SHORTCUT("selectionToolShortcut", defaultSelectionToolShortcut, selectionToolShortcutChanged)
}

QString ApplicationSettings::defaultToolModeShortcut() const
{
    return QLatin1String("T");
}

QString ApplicationSettings::toolModeShortcut() const
{
    GET_SHORTCUT("toolModeShortcut", defaultToolModeShortcut)
}

void ApplicationSettings::setToolModeShortcut(const QString &shortcut)
{
    SET_SHORTCUT("toolModeShortcut", defaultToolModeShortcut, toolModeShortcutChanged)
}

QString ApplicationSettings::defaultDecreaseToolSizeShortcut() const
{
    return QLatin1String("[");
}

QString ApplicationSettings::decreaseToolSizeShortcut() const
{
    GET_SHORTCUT("decreaseToolSizeShortcut", defaultDecreaseToolSizeShortcut)
}

void ApplicationSettings::setDecreaseToolSizeShortcut(const QString &shortcut)
{
    SET_SHORTCUT("decreaseToolSizeShortcut", defaultDecreaseToolSizeShortcut, decreaseToolSizeShortcutChanged)
}

QString ApplicationSettings::defaultIncreaseToolSizeShortcut() const
{
    return QLatin1String("]");
}

QString ApplicationSettings::increaseToolSizeShortcut() const
{
    GET_SHORTCUT("increaseToolSizeShortcut", defaultIncreaseToolSizeShortcut)
}

void ApplicationSettings::setIncreaseToolSizeShortcut(const QString &shortcut)
{
    SET_SHORTCUT("increaseToolSizeShortcut", defaultIncreaseToolSizeShortcut, increaseToolSizeShortcutChanged)
}

QString ApplicationSettings::defaultSwatchLeftShortcut() const
{
    return QLatin1String("A");
}

QString ApplicationSettings::swatchLeftShortcut() const
{
    GET_SHORTCUT("swatchLeftShortcut", defaultSwatchLeftShortcut)
}

void ApplicationSettings::setSwatchLeftShortcut(const QString &shortcut)
{
    SET_SHORTCUT("swatchLeftShortcut", defaultSwatchLeftShortcut, swatchLeftShortcutChanged)
}

QString ApplicationSettings::defaultSwatchRightShortcut() const
{
    return QLatin1String("D");
}

QString ApplicationSettings::swatchRightShortcut() const
{
    GET_SHORTCUT("swatchRightShortcut", defaultSwatchRightShortcut)
}

void ApplicationSettings::setSwatchRightShortcut(const QString &shortcut)
{
    SET_SHORTCUT("swatchRightShortcut", defaultSwatchRightShortcut, swatchRightShortcutChanged)
}

QString ApplicationSettings::defaultSwatchUpShortcut() const
{
    return QLatin1String("W");
}

QString ApplicationSettings::swatchUpShortcut() const
{
    GET_SHORTCUT("swatchUpShortcut", defaultSwatchUpShortcut)
}

void ApplicationSettings::setSwatchUpShortcut(const QString &shortcut)
{
    SET_SHORTCUT("swatchUpShortcut", defaultSwatchUpShortcut, swatchUpShortcutChanged)
}

QString ApplicationSettings::defaultSwatchDownShortcut() const
{
    return QLatin1String("S");
}

QString ApplicationSettings::swatchDownShortcut() const
{
    GET_SHORTCUT("swatchDownShortcut", defaultSwatchDownShortcut)
}

void ApplicationSettings::setSwatchDownShortcut(const QString &shortcut)
{
    SET_SHORTCUT("swatchDownShortcut", defaultSwatchDownShortcut, swatchDownShortcutChanged)
}

QString ApplicationSettings::defaultFullScreenToggleShortcut() const
{
    return QLatin1String("F11");
}

QString ApplicationSettings::fullScreenToggleShortcut() const
{
    GET_SHORTCUT("fullScreenToggleShortcut", defaultFullScreenToggleShortcut)
}

void ApplicationSettings::setFullScreenToggleShortcut(const QString &shortcut)
{
    SET_SHORTCUT("fullScreenToggleShortcut", defaultFullScreenToggleShortcut, fullScreenToggleShortcutChanged)
}
