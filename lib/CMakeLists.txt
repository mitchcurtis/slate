# lib/CMakeLists.txt

qt_add_qml_module(slate
    VERSION 1.0
    URI "App"
    SOURCES
        3rdparty/bitmap/bmp.h
        3rdparty/bitmap/bmp.c
        3rdparty/bitmap/misc/gif.h
        3rdparty/bitmap/misc/gif.c
        # For exporting GIFs.
        3rdparty/gif-h/qt-cpp/gifwriter.cpp
        3rdparty/gif-h/qt-cpp/gifwriter.h
        addanimationcommand.cpp
        addanimationcommand.h
        addguidescommand.cpp
        addguidescommand.h
        addlayercommand.cpp
        addlayercommand.h
        addnotecommand.cpp
        addnotecommand.h
        animation.cpp
        animation.h
        animationmodel.h
        animationmodel.cpp
        animationplayback.cpp
        animationplayback.h
        animationsystem.cpp
        animationsystem.h
        applicationsettings.cpp
        applicationsettings.h
        applygreedypixelfillcommand.cpp
        applygreedypixelfillcommand.h
        applypixelerasercommand.cpp
        applypixelerasercommand.h
        applypixelfillcommand.cpp
        applypixelfillcommand.h
        applypixellinecommand.cpp
        applypixellinecommand.h
        applypixelpencommand.cpp
        applypixelpencommand.h
        applytilecanvaspixelfillcommand.cpp
        applytilecanvaspixelfillcommand.h
        applytileerasercommand.cpp
        applytileerasercommand.h
        applytilefillcommand.cpp
        applytilefillcommand.h
        applytilepencommand.cpp
        applytilepencommand.h
        autoswatchmodel.cpp
        autoswatchmodel.h
        buildinfo.cpp
        buildinfo.h
        canvaspane.cpp
        canvaspane.h
        canvaspaneitem.cpp
        canvaspaneitem.h
        changeanimationordercommand.cpp
        changeanimationordercommand.h
        changeimagecanvassizecommand.cpp
        changeimagecanvassizecommand.h
        changeimagesizecommand.cpp
        changeimagesizecommand.h
        changelayeredimagecanvassizecommand.cpp
        changelayeredimagecanvassizecommand.h
        changelayeredimagesizecommand.cpp
        changelayeredimagesizecommand.h
        changelayernamecommand.cpp
        changelayernamecommand.h
        changelayeropacitycommand.cpp
        changelayeropacitycommand.h
        changelayerordercommand.cpp
        changelayerordercommand.h
        changelayervisiblecommand.cpp
        changelayervisiblecommand.h
        changenotecommand.cpp
        changenotecommand.h
        changetilecanvassizecommand.cpp
        changetilecanvassizecommand.h
        clipboard.h
        clipboard.cpp
        commands.h
        deleteanimationcommand.cpp
        deleteanimationcommand.h
        deleteguidescommand.cpp
        deleteguidescommand.h
        deleteimagecanvasselectioncommand.cpp
        deleteimagecanvasselectioncommand.h
        deletelayercommand.cpp
        deletelayercommand.h
        deletenotecommand.cpp
        deletenotecommand.h
        duplicateanimationcommand.cpp
        duplicateanimationcommand.h
        duplicatelayercommand.cpp
        duplicatelayercommand.h
        filevalidator.cpp
        filevalidator.h
        fillalgorithms.cpp
        fillalgorithms.h
        flipimagecanvasselectioncommand.cpp
        flipimagecanvasselectioncommand.h
        guide.cpp
        guide.h
        guidesitem.cpp
        guidesitem.h
        imagecanvas.cpp
        imagecanvas.h
        imagelayer.cpp
        imagelayer.h
        imageproject.cpp
        imageproject.h
        imageutils.h
        imageutils.cpp
        jsonutils.cpp
        jsonutils.h
        keysequenceeditor.cpp
        keysequenceeditor.h
        layeredimagecanvas.cpp
        layeredimagecanvas.h
        layeredimageproject.cpp
        layeredimageproject.h
        layermodel.cpp
        layermodel.h
        mergelayerscommand.cpp
        mergelayerscommand.h
        moveguidecommand.cpp
        moveguidecommand.h
        modifyanimationcommand.cpp
        modifyanimationcommand.h
        modifyimagecanvasselectioncommand.cpp
        modifyimagecanvasselectioncommand.h
        movelayeredimagecontentscommand.cpp
        movelayeredimagecontentscommand.h
        newprojectvalidator.cpp
        newprojectvalidator.h
        note.h
        note.cpp
        notesitem.h
        notesitem.cpp
        panedrawinghelper.cpp
        panedrawinghelper.h
        pasteacrosslayerscommand.cpp
        pasteacrosslayerscommand.h
        pasteimagecanvascommand.cpp
        pasteimagecanvascommand.h
        probabilityswatch.cpp
        probabilityswatch.h
        probabilityswatchmodel.cpp
        probabilityswatchmodel.h
        projectanimationhelper.cpp
        projectanimationhelper.h
        project.cpp
        project.h
        projectimageprovider.cpp
        projectimageprovider.h
        projectmanager.cpp
        projectmanager.h
        projectutils.cpp
        projectutils.h
        qtutils.h
        rearrangeimagecontentsintogridcommand.cpp
        rearrangeimagecontentsintogridcommand.h
        rearrangelayeredimagecontentsintogridcommand.cpp
        rearrangelayeredimagecontentsintogridcommand.h
        rectangularcursor.cpp
        rectangularcursor.h
        ruler.cpp
        ruler.h
        saturationlightnesspicker.cpp
        saturationlightnesspicker.h
        selectioncursorguide.cpp
        selectioncursorguide.h
        selectionitem.cpp
        selectionitem.h
        serialisablestate.cpp
        serialisablestate.h
        slate-global.h
        splitter.cpp
        splitter.h
        spriteimage.cpp
        spriteimage.h
        spriteimageprovider.cpp
        spriteimageprovider.h
        swatch.cpp
        swatch.h
        swatchcolour.cpp
        swatchcolour.h
        swatchmodel.cpp
        swatchmodel.h
        texturedfillparameters.cpp
        texturedfillparameters.h
        texturedfillpreviewitem.cpp
        texturedfillpreviewitem.h
        tile.cpp
        tile.h
        tilecanvas.cpp
        tilecanvas.h
        tilecanvaspaneitem.cpp
        tilecanvaspaneitem.h
        tilegrid.cpp
        tilegrid.h
        tileset.cpp
        tileset.h
        tilesetproject.cpp
        tilesetproject.h
        tilesetswatchimage.cpp
        tilesetswatchimage.h
        undocommand.h
        undocommand.cpp
)

find_package(Qt6 COMPONENTS Core)
find_package(Qt6 COMPONENTS Gui)
find_package(Qt6 COMPONENTS Qml)
find_package(Qt6 COMPONENTS Quick)
find_package(Qt6 COMPONENTS Widgets)

target_link_libraries(slate
    PUBLIC
        projectWarning
        Qt::Core
        Qt::Gui
        Qt::Qml
        Qt::Quick
        Qt::Widgets
)

target_include_directories(slate
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty
)

target_compile_definitions(slate
    PRIVATE
        SLATE_LIBRARY
)

set_target_properties(slate PROPERTIES
    CXX_EXTENSIONS FALSE
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED TRUE
)

# Generate vcs-repo-state.h
if(EXISTS ${CMAKE_SOURCE_DIR}/.git)
    find_package(Git)
    if(GIT_FOUND)
        EXECUTE_PROCESS(
            COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            OUTPUT_VARIABLE BUILD_VERSION
            OUTPUT_STRIP_TRAILING_WHITESPACE
            )

        configure_file(
            ${CMAKE_CURRENT_SOURCE_DIR}/vcs-repo-state.h.in
            ${CMAKE_CURRENT_BINARY_DIR}/vcs-repo-state.h
            )
    else()
        message(FATAL_ERROR "Can not find Git")
    endif()
else()
    message(FATAL_ERROR "Can not find .git repo")
endif()

