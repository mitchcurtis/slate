# Slate - Pixel Art Editor

Slate is a bitmap editor available for Linux, Windows and Mac. With Slate, you can:

- Edit images directly
- Use layers
- Create seamless 2D tilesets
- Preview sprite sheet animations

Slate was built for pixel art, and its goal is to simplify the creation of sprites and tilesets by offering a user-friendly, customisable interface.

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

Note that it's fine to ask questions here on GitHub as well - just [create an issue](https://github.com/mitchcurtis/slate/issues/new) and I will label it accordingly.

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
Open [slate.pro](https://github.com/mitchcurtis/slate/blob/master/slate.pro) in Qt Creator, configure the project using a kit with a Qt version that meets the requirement listed in the [Dependencies](#dependencies) section, and then build it.

#### Command Line ####

In-source builds:

    cd /path/to/slate-source-dir
    /path/to/qt/qmake
    make (or nmake/jom on Windows)

Shadow builds:

    mkdir slate-build
    cd slate-build
    /path/to/qt/qmake /path/to/slate-source-dir/slate.pro
    make (or nmake/jom on Windows)

---

The tileset in the screenshot was taken from the following page:

http://opengameart.org/content/lpc-tile-atlas
