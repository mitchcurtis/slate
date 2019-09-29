# Slate - Overview

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
# Contents

- [User Interface](#user-interface)
- [Project Types](#project-types)
  - [Images](#images)
  - [Layered Images](#layered-images)
  - [Tilesets](#tilesets)
  - [Comparison of Project Types](#comparison-of-project-types)
- [Features](#features)
  - [Animation](#animation)
  - [Textured Fill](#textured-fill)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

## User Interface

- [User Interface](https://github.com/mitchcurtis/slate/blob/master/doc/user-interface.md)

## Project Types

### Images

![Image Project icon](https://github.com/mitchcurtis/slate/blob/master/app/images/image-project.svg)

Slate supports editing bitmap images directly. This project type has no layer support.

### Layered Images

![Layered Image Project icon](https://github.com/mitchcurtis/slate/blob/master/app/images/layered-image-project.svg)

A layered image project can contain several images, each of which is stored as a layer. The highest layer in the list is drawn on top of the layers below it. This type of project can be exported as one or more images.

- [Getting Started with Layered Image Projects](https://github.com/mitchcurtis/slate/blob/master/doc/getting-started-layered.md)

### Tilesets

![Tileset Project icon](https://github.com/mitchcurtis/slate/blob/master/app/images/tileset-project.svg)

Tileset projects have a tileset bitmap image that can be edited by first painting tiles onto the grid, and then editing those tiles. An accompanying project file is created to save the contents of the grid. The tileset image is edited directly (i.e overwritten when the project is saved).

- [Getting Started with Tilesets](https://github.com/mitchcurtis/slate/blob/master/doc/getting-started-tileset.md)

### Comparison of Project Types

The following table lists features and which project types support them.

Feature | Images (.png) | Layered Images (.slp) | Tilesets (.stp)
------------ | ------------- | ------------- | -------------
Animation | :white_check_mark: <sup>1</sup> | :white_check_mark: | :x:
Grid | :x: | :x: | :white_check_mark:
Guides | :white_check_mark: <sup>1</sup> | :white_check_mark: | :white_check_mark:
Layers | :x: | :white_check_mark: | :x:
Resize Image | :white_check_mark: | :white_check_mark: | :x:
Selection Tool | :white_check_mark: | :white_check_mark: | :x:
Swatches | :white_check_mark: <sup>1</sup> | :white_check_mark: | :white_check_mark:
Textured Fill Tool | :white_check_mark: | :white_check_mark: | :x:
Tilesets | :x: |  :x: | :white_check_mark:

<sup>1</sup> - Data for this feature cannot be saved as there is no project file to save to.

## Features

### Animation

- [Animation Tutorial](https://github.com/mitchcurtis/slate/blob/master/doc/animation-tutorial.md)

### Textured Fill

- [Textured Fill Tool Tutorial](https://github.com/mitchcurtis/slate/blob/master/doc/textured-fill-tool-tutorial.md)
