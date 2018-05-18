# Slate - Getting Started with Layered Image Projects

## Creating a New Project

Whether you have an existing tileset or wish to create a new one from scratch, the first step is to create a new project. There are two ways to do this: Ctrl+N or via the File menu.

Next, select New Layered Image from the list:

<img src="https://github.com/mitchcurtis/slate/blob/master/doc/images/slate-new-project-dialog.png" width="600">

Choose a width and height for the canvas. You can also choose whether or not the first layer is transparent or filled with white.

<img src="https://github.com/mitchcurtis/slate/blob/master/doc/images/slate-new-layered-image-project-dialog.png" width="600">

Hit OK. You'll now be looking at a new layered image project:

<img src="https://github.com/mitchcurtis/slate/blob/master/doc/images/slate-new-layered-image-project.png" width="600">

On the right side of the screen, you'll see the Layer panel. This allows you to add, remove, rename, re-order and delete layers.

## Creating New Layers

In the bottom left of the Layer panel, click the "+" button to add a new layer. Afterwards, you will see two layers:

<img src="https://github.com/mitchcurtis/slate/blob/master/doc/images/slate-layered-image-project-new-layer.png" height="600">

## Renaming Layers

Double click on the layer to rename it.

<img src="https://github.com/mitchcurtis/slate/blob/master/doc/images/slate-layered-image-project-rename-layers.png" width="285">

## Exporting Layers

Layered image projects can be exported by File > Export or with Ctrl+Shift+E. If you'd like the project to be automatically exported every time you save, check the Auto Export menu item under the File menu. The image will be saved to a file with the same name as the project, except with a .png extension.

It's also possible to export one or more layers into a separate image by giving those layers a prefix. For example, in the image below, the layer at the top is given the prefix **[something]**, and will be saved as **something.png**:

<img src="https://github.com/mitchcurtis/slate/blob/master/doc/images/slate-layered-image-project-export-layers.png" height="600">

A common use case for exporting images is to use the name of the project and a suffix. The **%p** variable can be used for this purpose. It expands to the name of the project, and anything after it will be appended to the file name of the exported image. For example, if the project was named **my-project.slp**, the layers named **[%p-content] left** and **[%p-content] right** would be exported as an image named **my-project-content.png**. Anything after the prefix is ignored and only serves to differentiate layers with the same prefix.

It is also possible to use the **[no-export]** prefix to prevent a layer from being exported. This is useful if you are experimenting with something and need to see how it looks without affecting the exported images.
