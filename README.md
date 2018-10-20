# Slate - Pixel Art Editor

<p align="center">
  <img src="https://github.com/mitchcurtis/slate/blob/master/app/images/logo/slate-icon-web.svg" />
</p>

Slate is a bitmap editor available for Linux, Windows and Mac. With Slate, you can:

- Edit images directly
- Use layers
- Create seamless 2D tilesets
- Preview sprite sheet animations

Slate was built for pixel art, and its goal is to simplify the creation of sprites and tilesets by offering a user-friendly, customisable interface.

<!--
    Note: generate the table of contents for each file with the following commands:

    cd ~/dev/slate
    doctoc . --github --title '# Contents'
-->

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
# Contents

  - [Screenshots](#screenshots)
  - [Downloads](#downloads)
  - [Installing](#installing)
  - [Documentation](#documentation)
  - [Discussion](#discussion)
  - [Notes](#notes)
- [Building From Source](#building-from-source)
  - [Dependencies](#dependencies)
  - [Cloning](#cloning)
  - [Building](#building)
    - [Qt Creator](#qt-creator)
    - [Command Line](#command-line)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

### Screenshots

![Screenshot of Slate on Linux](https://github.com/mitchcurtis/slate/blob/master/doc/images/slate-v0.1.0-tileset-linux.png "Screenshot of Slate on Linux")

![Screenshot of Slate v0.1.0 on Mac](https://github.com/mitchcurtis/slate/blob/master/doc/images/slate-v0.1.0-mac-layers.png "Screenshot of Slate v0.1.0 on Mac")

![Screenshot of Slate v0.0.10 on Mac](https://github.com/mitchcurtis/slate/blob/master/doc/images/slate-v0.0.10-mac.png "Screenshot of Slate v0.0.10 on Mac")

### Downloads ###

See the [releases](https://github.com/mitchcurtis/slate/releases) page for the available downloads.

### Installing ###

All Slate builds are portable, meaning that no installation is necessary. On Windows, however, it may be necessary to run the vc_redist.x64.exe file that comes with the download in order to install certain Microsoft C++ libraries if you've never installed any applications that require this prior to running Slate.

### Documentation ###
- [Overview](https://github.com/mitchcurtis/slate/blob/master/doc/overview.md)

### Discussion ###

Join Slate's Discord channel for discussion: https://discord.gg/ZtAAg5D

It's fine to ask questions here on GitHub as well - just [create an issue](https://github.com/mitchcurtis/slate/issues/new) and I will label it accordingly.

### Notes ###
- This is a work in progress, but should already be usable. Feel free to [report](https://github.com/mitchcurtis/slate/issues) any bugs you find. :)
- The UI is currently designed for the [Material style](http://doc.qt.io/qt-5/qtquickcontrols2-material.html), and will probably look bad when run with others.

## Building From Source ##

### Dependencies ###

* Qt 5.12

### Cloning ###

`master` is the default branch where all work is done, and is not guaranteed to be stable. I [tag](https://github.com/mitchcurtis/slate/tags) releases, so if you're looking for a stable build to try out, it's best to pick the latest one from `git tag -l`. 

### Building ###

There are two options for building:

#### Qt Creator ####

It's recommended to build using Qt Creator for convenience.

Open [slate.qbs](https://github.com/mitchcurtis/slate/blob/master/slate.qbs) in Qt Creator, configure the project using a kit with a Qt version that meets the requirement listed in the [Dependencies](#dependencies) section, and then build it.

#### Command Line ####

First, follow Qbs' [setup guide](http://doc.qt.io/qbs/setup.html). Once you've set it up, choose one of the following approaches.

In-source builds:

    cd /path/to/slate-source-dir
    qbs

Shadow builds:

    mkdir slate-build
    cd slate-build
    qbs /path/to/slate-source-dir/slate.qbs
    
For more information about building Qbs applications, see [this page](http://doc.qt.io/qbs/building-applications.html).

---

The tileset in the screenshot was taken from the following page:

http://opengameart.org/content/lpc-tile-atlas
