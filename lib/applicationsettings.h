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

#ifndef APPLICATIONSETTINGS_H
#define APPLICATIONSETTINGS_H

#include <QColor>
#include <QSettings>
#include <QUrl>

#include "slate-global.h"

/*
    When adding a new configurable shortcut, update the following:

    - OptionsDialog.qml
    - Shortcuts.qml
*/

class SLATE_EXPORT ApplicationSettings : public QSettings
{
    Q_OBJECT
    Q_PROPERTY(bool loadLastOnStartup READ loadLastOnStartup WRITE setLoadLastOnStartup NOTIFY loadLastOnStartupChanged)
    Q_PROPERTY(QStringList recentFiles READ recentFiles NOTIFY recentFilesChanged)
    Q_PROPERTY(bool gridVisible READ isGridVisible WRITE setGridVisible NOTIFY gridVisibleChanged)
    Q_PROPERTY(bool rulersVisible READ areRulersVisible WRITE setRulersVisible NOTIFY rulersVisibleChanged)
    Q_PROPERTY(bool guidesVisible READ areGuidesVisible WRITE setGuidesVisible NOTIFY guidesVisibleChanged)
    Q_PROPERTY(bool guidesLocked READ areGuidesLocked WRITE setGuidesLocked NOTIFY guidesLockedChanged)
    Q_PROPERTY(bool scrollZoom READ scrollZoom WRITE setScrollZoom NOTIFY scrollZoomChanged)
    Q_PROPERTY(bool fpsVisible READ isFpsVisible WRITE setFpsVisible NOTIFY fpsVisibleChanged)
    Q_PROPERTY(bool gesturesEnabled READ areGesturesEnabled WRITE setGesturesEnabled NOTIFY gesturesEnabledChanged)
    Q_PROPERTY(bool autoSwatchEnabled READ isAutoSwatchEnabled WRITE setAutoSwatchEnabled NOTIFY autoSwatchEnabledChanged)
    Q_PROPERTY(bool alwaysShowCrosshair READ isAlwaysShowCrosshair WRITE setAlwaysShowCrosshair NOTIFY alwaysShowCrosshairChanged)
    Q_PROPERTY(qreal windowOpacity READ windowOpacity WRITE setWindowOpacity NOTIFY windowOpacityChanged)
    Q_PROPERTY(QColor checkerColour1 READ checkerColour1 WRITE setCheckerColour1 NOTIFY checkerColour1Changed)
    Q_PROPERTY(QColor checkerColour2 READ checkerColour2 WRITE setCheckerColour2 NOTIFY checkerColour2Changed)
    Q_PROPERTY(int penToolRightClickBehaviour READ penToolRightClickBehaviour WRITE setPenToolRightClickBehaviour NOTIFY penToolRightClickBehaviourChanged)

    Q_PROPERTY(QString quitShortcut READ quitShortcut WRITE setQuitShortcut NOTIFY quitShortcutChanged)
    Q_PROPERTY(QString newShortcut READ newShortcut WRITE setNewShortcut NOTIFY newShortcutChanged)
    Q_PROPERTY(QString openShortcut READ openShortcut WRITE setOpenShortcut NOTIFY openShortcutChanged)
    Q_PROPERTY(QString saveShortcut READ saveShortcut WRITE setSaveShortcut NOTIFY saveShortcutChanged)
    Q_PROPERTY(QString saveAsShortcut READ saveAsShortcut WRITE setSaveAsShortcut NOTIFY saveAsShortcutChanged)
    Q_PROPERTY(QString exportShortcut READ exportShortcut WRITE setExportShortcut NOTIFY exportShortcutChanged)
    Q_PROPERTY(QString closeShortcut READ closeShortcut WRITE setCloseShortcut NOTIFY closeShortcutChanged)
    Q_PROPERTY(QString revertShortcut READ revertShortcut WRITE setRevertShortcut NOTIFY revertShortcutChanged)
    Q_PROPERTY(QString undoShortcut READ undoShortcut WRITE setUndoShortcut NOTIFY undoShortcutChanged)
    Q_PROPERTY(QString redoShortcut READ redoShortcut WRITE setRedoShortcut NOTIFY redoShortcutChanged)
    Q_PROPERTY(QString flipHorizontallyShortcut READ flipHorizontallyShortcut WRITE setFlipHorizontallyShortcut NOTIFY flipHorizontallyShortcutChanged)
    Q_PROPERTY(QString flipVerticallyShortcut READ flipVerticallyShortcut WRITE setFlipVerticallyShortcut NOTIFY flipVerticallyShortcutChanged)
    Q_PROPERTY(QString resizeCanvasShortcut READ resizeCanvasShortcut WRITE setResizeCanvasShortcut NOTIFY resizeCanvasShortcutChanged)
    Q_PROPERTY(QString resizeImageShortcut READ resizeImageShortcut WRITE setResizeImageShortcut NOTIFY resizeImageShortcutChanged)
    Q_PROPERTY(QString moveContentsShortcut READ moveContentsShortcut WRITE setMoveContentsShortcut NOTIFY moveContentsShortcutChanged)
    Q_PROPERTY(QString gridVisibleShortcut READ gridVisibleShortcut WRITE setGridVisibleShortcut NOTIFY gridVisibleShortcutChanged)
    Q_PROPERTY(QString rulersVisibleShortcut READ rulersVisibleShortcut WRITE setRulersVisibleShortcut NOTIFY rulersVisibleShortcutChanged)
    Q_PROPERTY(QString guidesVisibleShortcut READ guidesVisibleShortcut WRITE setGuidesVisibleShortcut NOTIFY guidesVisibleShortcutChanged)
    Q_PROPERTY(QString centreShortcut READ centreShortcut WRITE setCentreShortcut NOTIFY centreShortcutChanged)
    Q_PROPERTY(QString zoomInShortcut READ zoomInShortcut WRITE setZoomInShortcut NOTIFY zoomInShortcutChanged)
    Q_PROPERTY(QString zoomOutShortcut READ zoomOutShortcut WRITE setZoomOutShortcut NOTIFY zoomOutShortcutChanged)
    Q_PROPERTY(QString splitScreenShortcut READ splitScreenShortcut WRITE setSplitScreenShortcut NOTIFY splitScreenShortcutChanged)
    Q_PROPERTY(QString splitterLockedShortcut READ splitterLockedShortcut WRITE setSplitterLockedShortcut NOTIFY splitterLockedShortcutChanged)
    Q_PROPERTY(QString animationPlaybackShortcut READ animationPlaybackShortcut WRITE setAnimationPlaybackShortcut NOTIFY animationPlaybackShortcutChanged)
    Q_PROPERTY(QString optionsShortcut READ optionsShortcut WRITE setOptionsShortcut NOTIFY optionsShortcutChanged)
    Q_PROPERTY(QString penToolShortcut READ penToolShortcut WRITE setPenToolShortcut NOTIFY penToolShortcutChanged)
    Q_PROPERTY(QString eyeDropperToolShortcut READ eyeDropperToolShortcut WRITE setEyeDropperToolShortcut NOTIFY eyeDropperToolShortcutChanged)
    Q_PROPERTY(QString eraserToolShortcut READ eraserToolShortcut WRITE setEraserToolShortcut NOTIFY eraserToolShortcutChanged)
    Q_PROPERTY(QString fillToolShortcut READ fillToolShortcut WRITE setFillToolShortcut NOTIFY fillToolShortcutChanged)
    Q_PROPERTY(QString selectionToolShortcut READ selectionToolShortcut WRITE setSelectionToolShortcut NOTIFY selectionToolShortcutChanged)
    Q_PROPERTY(QString toolModeShortcut READ toolModeShortcut WRITE setToolModeShortcut NOTIFY toolModeShortcutChanged)
    Q_PROPERTY(QString decreaseToolSizeShortcut READ decreaseToolSizeShortcut WRITE setDecreaseToolSizeShortcut NOTIFY decreaseToolSizeShortcutChanged)
    Q_PROPERTY(QString increaseToolSizeShortcut READ increaseToolSizeShortcut WRITE setIncreaseToolSizeShortcut NOTIFY increaseToolSizeShortcutChanged)
    Q_PROPERTY(QString swatchLeftShortcut READ swatchLeftShortcut WRITE setSwatchLeftShortcut NOTIFY swatchLeftShortcutChanged)
    Q_PROPERTY(QString swatchRightShortcut READ swatchRightShortcut WRITE setSwatchRightShortcut NOTIFY swatchRightShortcutChanged)
    Q_PROPERTY(QString swatchUpShortcut READ swatchUpShortcut WRITE setSwatchUpShortcut NOTIFY swatchUpShortcutChanged)
    Q_PROPERTY(QString swatchDownShortcut READ swatchDownShortcut WRITE setSwatchDownShortcut NOTIFY swatchDownShortcutChanged)
    Q_PROPERTY(QString fullScreenToggleShortcut READ fullScreenToggleShortcut WRITE setFullScreenToggleShortcut NOTIFY fullScreenToggleShortcutChanged)

public:
    explicit ApplicationSettings(QObject *parent = 0);

    bool loadLastOnStartup() const;
    void setLoadLastOnStartup(bool loadLastOnStartup);
    bool defaultLoadLastOnStartup() const;

    QStringList recentFiles() const;
    void addRecentFile(const QString &filePath);
    Q_INVOKABLE void clearRecentFiles();
    void removeInvalidRecentFiles();
    // Converts the paths we store ("file:///some-file.png") into a user-facing path.
    Q_INVOKABLE QString displayableFilePath(const QString &filePath) const;

    bool isGridVisible() const;
    void setGridVisible(bool isGridVisible);
    bool defaultGridVisible() const;

    bool areRulersVisible() const;
    void setRulersVisible(bool rulersVisible);
    bool defaultRulersVisible() const;

    bool areGuidesVisible() const;
    void setGuidesVisible(bool guidesVisible);
    bool defaultGuidesVisible() const;

    bool areGuidesLocked() const;
    void setGuidesLocked(bool guidesLocked);
    bool defaultGuidesLocked() const;

    bool defaultScrollZoom() const;
    bool scrollZoom() const;
    void setScrollZoom(bool scrollZoom);

    bool defaultFpsVisible() const;
    bool isFpsVisible() const;
    void setFpsVisible(bool fpsVisible);

    bool defaultGesturesEnabled() const;
    bool areGesturesEnabled() const;
    void setGesturesEnabled(bool gesturesEnabled);

    bool defaultAutoSwatchEnabled() const;
    bool isAutoSwatchEnabled() const;
    void setAutoSwatchEnabled(bool autoSwatchEnabled);

    bool defaultAlwaysShowCrosshair() const;
    bool isAlwaysShowCrosshair() const;
    void setAlwaysShowCrosshair(bool alwaysShowCrosshair);

    qreal defaultWindowOpacity() const;
    qreal windowOpacity() const;
    void setWindowOpacity(qreal opacity);

    QColor defaultCheckerColour1() const;
    QColor checkerColour1() const;
    void setCheckerColour1(const QColor &colour);

    QColor defaultCheckerColour2() const;
    QColor checkerColour2() const;
    void setCheckerColour2(const QColor &colour);

    int defaultPenToolRightClickBehaviour() const;
    int penToolRightClickBehaviour() const;
    void setPenToolRightClickBehaviour(int penToolRightClickBehaviour);

    Q_INVOKABLE void resetShortcutsToDefaults();

    QString defaultQuitShortcut() const;
    QString quitShortcut() const;
    void setQuitShortcut(const QString &shortcut);

    QString defaultNewShortcut() const;
    QString newShortcut() const;
    void setNewShortcut(const QString &shortcut);

    QString defaultOpenShortcut() const;
    QString openShortcut() const;
    void setOpenShortcut(const QString &shortcut);

    QString defaultSaveShortcut() const;
    QString saveShortcut() const;
    void setSaveShortcut(const QString &shortcut);

    QString defaultSaveAsShortcut() const;
    QString saveAsShortcut() const;
    void setSaveAsShortcut(const QString &shortcut);

    QString defaultExportShortcut() const;
    QString exportShortcut() const;
    void setExportShortcut(const QString &shortcut);

    QString defaultCloseShortcut() const;
    QString closeShortcut() const;
    void setCloseShortcut(const QString &shortcut);

    QString defaultRevertShortcut() const;
    QString revertShortcut() const;
    void setRevertShortcut(const QString &shortcut);

    QString defaultUndoShortcut() const;
    QString undoShortcut() const;
    void setUndoShortcut(const QString &shortcut);

    QString defaultRedoShortcut() const;
    QString redoShortcut() const;
    void setRedoShortcut(const QString &shortcut);

    QString defaultFlipHorizontallyShortcut() const;
    QString flipHorizontallyShortcut() const;
    void setFlipHorizontallyShortcut(const QString &shortcut);

    QString defaultFlipVerticallyShortcut() const;
    QString flipVerticallyShortcut() const;
    void setFlipVerticallyShortcut(const QString &shortcut);

    QString defaultResizeCanvasShortcut() const;
    QString resizeCanvasShortcut() const;
    void setResizeCanvasShortcut(const QString &shortcut);

    QString defaultResizeImageShortcut() const;
    QString resizeImageShortcut() const;
    void setResizeImageShortcut(const QString &shortcut);

    QString defaultMoveContentsShortcut() const;
    QString moveContentsShortcut() const;
    void setMoveContentsShortcut(const QString &shortcut);

    QString defaultGridVisibleShortcut() const;
    QString gridVisibleShortcut() const;
    void setGridVisibleShortcut(const QString &shortcut);

    QString defaultRulersVisibleShortcut() const;
    QString rulersVisibleShortcut() const;
    void setRulersVisibleShortcut(const QString &shortcut);

    QString defaultGuidesVisibleShortcut() const;
    QString guidesVisibleShortcut() const;
    void setGuidesVisibleShortcut(const QString &shortcut);

    QString defaultCentreShortcut() const;
    QString centreShortcut() const;
    void setCentreShortcut(const QString &shortcut);

    QString defaultZoomInShortcut() const;
    QString zoomInShortcut() const;
    void setZoomInShortcut(const QString &shortcut);

    QString defaultZoomOutShortcut() const;
    QString zoomOutShortcut() const;
    void setZoomOutShortcut(const QString &shortcut);

    QString defaultSplitScreenShortcut() const;
    QString splitScreenShortcut() const;
    void setSplitScreenShortcut(const QString &shortcut);

    QString defaultSplitterLockedShortcut() const;
    QString splitterLockedShortcut() const;
    void setSplitterLockedShortcut(const QString &shortcut);

    QString defaultAnimationPlaybackShortcut() const;
    QString animationPlaybackShortcut() const;
    void setAnimationPlaybackShortcut(const QString &shortcut);

    QString defaultOptionsShortcut() const;
    QString optionsShortcut() const;
    void setOptionsShortcut(const QString &shortcut);

    QString defaultPenToolShortcut() const;
    QString penToolShortcut() const;
    void setPenToolShortcut(const QString &shortcut);

    QString defaultEyeDropperToolShortcut() const;
    QString eyeDropperToolShortcut() const;
    void setEyeDropperToolShortcut(const QString &shortcut);

    QString defaultFillToolShortcut() const;
    QString fillToolShortcut() const;
    void setFillToolShortcut(const QString &shortcut);

    QString defaultEraserToolShortcut() const;
    QString eraserToolShortcut() const;
    void setEraserToolShortcut(const QString &shortcut);

    QString defaultSelectionToolShortcut() const;
    QString selectionToolShortcut() const;
    void setSelectionToolShortcut(const QString &shortcut);

    QString defaultToolModeShortcut() const;
    QString toolModeShortcut() const;
    void setToolModeShortcut(const QString &shortcut);

    QString defaultDecreaseToolSizeShortcut() const;
    QString decreaseToolSizeShortcut() const;
    void setDecreaseToolSizeShortcut(const QString &shortcut);

    QString defaultIncreaseToolSizeShortcut() const;
    QString increaseToolSizeShortcut() const;
    void setIncreaseToolSizeShortcut(const QString &shortcut);

    QString defaultSwatchLeftShortcut() const;
    QString swatchLeftShortcut() const;
    void setSwatchLeftShortcut(const QString &shortcut);

    QString defaultSwatchRightShortcut() const;
    QString swatchRightShortcut() const;
    void setSwatchRightShortcut(const QString &shortcut);

    QString defaultSwatchUpShortcut() const;
    QString swatchUpShortcut() const;
    void setSwatchUpShortcut(const QString &shortcut);

    QString defaultSwatchDownShortcut() const;
    QString swatchDownShortcut() const;
    void setSwatchDownShortcut(const QString &shortcut);

    QString defaultFullScreenToggleShortcut() const;
    QString fullScreenToggleShortcut() const;
    void setFullScreenToggleShortcut(const QString &shortcut);

signals:
    void loadLastOnStartupChanged();
    void recentFilesChanged();
    void gridVisibleChanged();
    void rulersVisibleChanged();
    void guidesVisibleChanged();
    void guidesLockedChanged();
    void scrollZoomChanged();
    void fpsVisibleChanged();
    void gesturesEnabledChanged();
    void autoSwatchEnabledChanged();
    void alwaysShowCrosshairChanged();
    void windowOpacityChanged();
    void checkerColour1Changed();
    void checkerColour2Changed();
    void penToolRightClickBehaviourChanged();

    void quitShortcutChanged();
    void newShortcutChanged();
    void openShortcutChanged();
    void saveShortcutChanged();
    void saveAsShortcutChanged();
    void exportShortcutChanged();
    void closeShortcutChanged();
    void revertShortcutChanged();
    void undoShortcutChanged();
    void redoShortcutChanged();
    void flipHorizontallyShortcutChanged();
    void flipVerticallyShortcutChanged();
    void resizeCanvasShortcutChanged();
    void resizeImageShortcutChanged();
    void moveContentsShortcutChanged();
    void gridVisibleShortcutChanged();
    void rulersVisibleShortcutChanged();
    void guidesVisibleShortcutChanged();
    void centreShortcutChanged();
    void zoomInShortcutChanged();
    void zoomOutShortcutChanged();
    void splitScreenShortcutChanged();
    void splitterLockedShortcutChanged();
    void animationPlaybackShortcutChanged();
    void penToolShortcutChanged();
    void optionsShortcutChanged();
    void eyeDropperToolShortcutChanged();
    void fillToolShortcutChanged();
    void eraserToolShortcutChanged();
    void selectionToolShortcutChanged();
    void toolModeShortcutChanged();
    void decreaseToolSizeShortcutChanged();
    void increaseToolSizeShortcutChanged();
    void swatchLeftShortcutChanged();
    void swatchRightShortcutChanged();
    void swatchUpShortcutChanged();
    void swatchDownShortcutChanged();
    void fullScreenToggleShortcutChanged();
};

#endif // APPLICATIONSETTINGS_H
