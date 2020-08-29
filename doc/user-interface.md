# Slate - User Interface

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
# Contents

- [Reference](#reference)
  - [Tool Bar](#tool-bar)
  - [Colour](#colour)
  - [Swatches](#swatches)
  - [Layers](#layers)
  - [Animation](#animation)
- [Settings](#settings)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

## Reference

### Tool Bar

![Tool bar](https://github.com/mitchcurtis/slate/blob/release/doc/images/slate-tool-bar.png)

1. Changes the size of the canvas.
2. Changes the size of the image.
3. Crops the image to the current selection.
4. Undo the last canvas operation.
5. Redo the last undone canvas operation
6. Switches the current tool to the pen tool.
7. Switches the current tool to the eye dropper (colour picker) tool.
8. Switches the current tool to the eraser tool.
9. Switches the current tool to the fill tool. Long press to bring up the fill tool menu.
10. Switches the current tool to the selection tool.
11. Switches the current tool to the note tool.
12. Changes the size of drawing tools.
13. Changes the shape of drawing tools.
14. Rotates the selection by 90 degrees counter-clockwise.
15. Rotates the selection by 90 degrees clockwise.
16. Flips the selection horizontally.
17. Flips the selection vertically.
18. Shows rulers.
19. Shows guides.
20. Locks guides.
21. Shows notes.
22. Toggles split screen.
23. Locks split screen splitter.
24. Toggles full screen.

### Colour

![Colour panel](https://github.com/mitchcurtis/slate/blob/release/doc/images/slate-colour-panel.png)

1. Toggles the visibility of the panel.
2. Switches between foreground and background colours.
3. Shows the current colour as a hex value. Can be edited.
4. Changes the hue of the current colour.
5. Changes the saturation and lightness of the current colour.
6. Changes the opacity of the current colour.
7. Decreases the lightness of the current colour.
8. Increases the lightness of the current colour.
9. Decreases the saturation of the current colour.
10. Increases the saturation of the current colour.

### Swatches

![Swatches panel](https://github.com/mitchcurtis/slate/blob/release/doc/images/slate-swatches-panel.png)

1. Toggles the visibility of the panel.
2. Displays auto swatch colours if enabled (**Settings > Enable auto swatch (experimental)**).
3. Displays custom swatch colours.
4. Creates a new custom swatch from the current colour.

### Layers

![Layers panel](https://github.com/mitchcurtis/slate/blob/release/doc/images/slate-layers-panel.png)

1. Toggles the visibility of the panel.
2. Displays layers. Each layer can be hidden by clicking on the icon, and renamed by double-clicking on its name.
3. Creates a new layer.
4. Moves the current layer down.
5. Moves the current layer up.
6. Duplicates the current layer.
7. Deletes the current layer.

### Animation

![Animation panel](https://github.com/mitchcurtis/slate/blob/release/doc/images/slate-animation-panel.png)

1. Opens animation playback settings.
2. Toggles the visibility of the panel.
3. Displays a preview of the animation. The scale of the preview can be changed in the settings.
4. Plays/pauses the animation preview playback.
5. Displays the progress of the animation preview playback.
6. Toggles between playing once and looping.
7. Exports the animation as a GIF.
8. Displays animations. Each animation can be previewed by clicking on it, renamed by double-clicking on it, and configured by clicking the configure button.
9. Creates a new animation.
10. Moves the current animation down.
11. Moves the current animation up.
12. Duplicates the current animation.
13. Deletes the current animation.

## Settings

The following user interface settings are saved to the project file when it is saved:

* The last fill tool that was used.
* Pane settings: the width, zoom level, and position of each pane.
* Split screen settings: enabled and locked.
* The position of the view in the Layers panel.
* Split view settings: the width of the canvas and the panel, along with the panels it contains (Colour panel, Swatches panel, etc).
