<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
# Contents

- [Slate - Textured Fill Tool Tutorial](#slate---textured-fill-tool-tutorial)
  - [Introduction](#introduction)
  - [Variance](#variance)
  - [Swatch](#swatch)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

# Slate - Textured Fill Tool Tutorial

## Introduction

This tutorial covers how to use the Textured Fill tool. The Textured Fill tool is similar to the regular Fill Tool in that it allows you to fill an area. The difference is that the area is filled with pixels randomly chosen based on a set of parameters. There are two types of parameters: variance and swatch-based. In this tutorial, we will cover both types by creating a road texture for an isometric tileset.

The initial project can be downloaded [here](https://github.com/mitchcurtis/slate/blob/master/tests/manual/screenshots/resources/textured-fill-tool-tutorial-1.slp).

When opened, you should see the following:

![Chapter 1 screenshot](https://github.com/mitchcurtis/slate/blob/release/doc/images/slate-textured-fill-tool-tutorial-1.png)

## Variance

The Variance textured fill type allows you to fill an area with the foreground colour. In additon, the hue, saturation and lightness of each pixel is randomly adjusted based on the textured fill settings.

To start with, select the grey colour using the eyedropper tool. Then, open the Textured Fill settings dialog from the Edit menu.

![Chapter 1.1 screenshot](https://github.com/mitchcurtis/slate/blob/release/doc/images/slate-textured-fill-tool-tutorial-1.1.png)

By default, the Lightness modifier is enabled, and that's the one we'll use for our tile.

Adjust the Lightness variance range slider so that both handles roughly match the positions in the image below:

![Chapter 1.2 screenshot](https://github.com/mitchcurtis/slate/blob/release/doc/images/slate-textured-fill-tool-tutorial-1.2.png)

As you release each handle, you can see the preview update. It can sometimes be hard to see what's going on when zoomed out, so increase the preview scale with the slider:

![Chapter 1.3 screenshot](https://github.com/mitchcurtis/slate/blob/release/doc/images/slate-textured-fill-tool-tutorial-1.3.png)

Click OK to confirm the settings. Then, switch to the Textured Fill tool by pressing Shift + G or long-pressing the fill tool button and selecting the relevant menu item. Click in the tile to fill it. You will see something similar to this:

![Chapter 1.4 screenshot](https://github.com/mitchcurtis/slate/blob/release/doc/images/slate-textured-fill-tool-tutorial-1.4.png)

Zoom out and centre the view (Ctrl + Space or View > Centre) to see it from another perspective:

![Chapter 1.5 screenshot](https://github.com/mitchcurtis/slate/blob/release/doc/images/slate-textured-fill-tool-tutorial-1.5.png)

## Swatch

The Swatch textured fill type allows you to fill an area with colours from a swatch. This is a good tool to use if you have a specific colour palette that you want to follow.

In the [project](https://github.com/mitchcurtis/slate/blob/master/tests/manual/screenshots/resources/slate-textured-fill-tool-tutorial-2.slp) shown in the image below, we've already started to draw some colours that we like onto the tile.

![Chapter 2 screenshot](https://github.com/mitchcurtis/slate/blob/release/doc/images/slate-textured-fill-tool-tutorial-2.png)

We can now select those pixels in preparation for adding them to the swatch:

![Chapter 2.1 screenshot](https://github.com/mitchcurtis/slate/blob/release/doc/images/slate-textured-fill-tool-tutorial-2.1.png)

In the Edit menu, click "Add to Textured Fill Swatch...". This will add the colours (automatically removing any duplicates) to the Textured Fill swatch and then open the relevant settings:

![Chapter 2.2 screenshot](https://github.com/mitchcurtis/slate/blob/release/doc/images/slate-textured-fill-tool-tutorial-2.2.png)

Now we can see each colour in the swatch, along with its probability of being used for each pixel. By default, each colour has a probability equal to its occurrence in the original selection. This is very useful for creating variations of an existing texture, for example.

Let's reduce the frequency of the yellow-ish colour. Scroll down the list to find it, and then adjust its probability slider as shown:

![Chapter 2.3 screenshot](https://github.com/mitchcurtis/slate/blob/release/doc/images/slate-textured-fill-tool-tutorial-2.3.png)

Now that we're happy with how the preview looks, click OK to confirm the settings. Then, click in the tile with the textured fill tool to fill it:

![Chapter 2.4 screenshot](https://github.com/mitchcurtis/slate/blob/release/doc/images/slate-textured-fill-tool-tutorial-2.4.png)

Zoom out and centre the view (Ctrl + Space or View > Centre) to see it from another perspective:

![Chapter 2.5 screenshot](https://github.com/mitchcurtis/slate/blob/release/doc/images/slate-textured-fill-tool-tutorial-2.5.png)
