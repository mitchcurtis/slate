# Slate Tileset Editor

A simple bitmap editor for 2D tilesets, written with [Qt Quick Controls 2](http://doc.qt.io/qt-5/qtquickcontrols2-index.html).

Slate's goal is to simplify the creation of seamless tilesets.

![Slate Screenshot](https://github.com/mitchcurtis/slate/blob/master/slate.png "Slate")

### Documentation ###
- [Getting Started](https://github.com/mitchcurtis/slate/blob/master/doc/getting-started.md)

### Notes ###
- This is a work in progress, but should already be usable. Feel free to report any bugs you find. :)
- The UI is currently designed for the [Material style](http://doc.qt.io/qt-5/qtquickcontrols2-material.html), and will probably look bad when run with others.
- Windows Binaries are coming soon. Other platforms (Linux, Mac) will follow once I've tested on them.

### Dependencies ###

* Qt >= 5.8
* https://github.com/mitchcurtis/qt-undo
* https://github.com/mitchcurtis/pickawinner

qt-undo and pickawinner can be cloned and built separately, but they are also available as submodules of Slate's repo, and can therefore be built automatically when
Slate is built, by first running the following commands:

cd <slate-source-dir>
git submodule init
git submodule update

---

The tileset in the screenhot was taken from the following page:

http://opengameart.org/content/lpc-tile-atlas
