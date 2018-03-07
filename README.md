# Slate - Pixel Art Editor

Slate is a bitmap editor available for Linux, Windows and Mac. With Slate, you can:

- Edit images directly
- Use layers
- Create seamless 2D tilesets
- Preview sprite sheet animations

Slate was built for pixel art, and its goal is to simplify the creation of sprites and tilesets by offering a user-friendly, customisable interface.

![Screenshot of Slate on Windows](https://github.com/mitchcurtis/slate/blob/master/doc/images/slate-tileset-windows.png "Screenshot of Slate on Windows")

![Screenshot of Slate v0.0.8 on Mac](https://github.com/mitchcurtis/slate/blob/master/doc/images/slate-v0.0.8-mac.png "Screenshot of Slate v0.0.8 on Mac")

![Screenshot of Slate v0.0.10 on Mac](https://github.com/mitchcurtis/slate/blob/master/doc/images/slate-v0.0.10-mac.png "Screenshot of Slate v0.0.10 on Mac")

### Downloads ###

See the [releases](https://github.com/mitchcurtis/slate/releases) page for the available downloads.

### Documentation ###
- [Getting Started](https://github.com/mitchcurtis/slate/blob/master/doc/getting-started.md)

### Notes ###
- This is a work in progress, but should already be usable. Feel free to report any bugs you find. :)
- The UI is currently designed for the [Material style](http://doc.qt.io/qt-5/qtquickcontrols2-material.html), and will probably look bad when run with others.

### Dependencies ###

* Qt >= 5.11
* https://github.com/mitchcurtis/qt-undo
* https://github.com/mitchcurtis/pickawinner

### Building ###

qt-undo and pickawinner can be cloned and built separately, but they are also available as submodules of Slate's repo, and can therefore be built automatically when
Slate is built, by first running the following commands:

    cd /path/to/slate-source-dir
    git submodule init
    git submodule update
    
Once that's set up, there are two options for building:

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
