# app/CMakeLists.txt

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick Widgets)

qt_add_executable(app
    application.h
    application.cpp
    main.cpp

    config.qrc
    fonts/fonts.qrc
    images/images.qrc

    qml/qml.qrc
)

# Ensure that Creator's QML code model can find the Slate QML module.
file(REAL_PATH "../qml" LIB_PATH BASE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
list(APPEND QML_IMPORT_PATHS "${LIB_PATH}")
set(QML_IMPORT_PATH "${QML_IMPORT_PATHS}" CACHE STRING "Qt Creator QML import paths")

set_target_properties(app PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER mitchcurtis
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_compile_definitions(app
    PRIVATE
        APP_VERSION="${PROJECT_VERSION}"
)

target_compile_definitions(app
    PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

target_link_libraries(app
    PRIVATE
        slate
        projectWarning
        Qt::Core
        Qt::Gui
        Qt::Qml
        Qt::Quick
        Qt::Widgets
)

set_target_properties(
    app
    PROPERTIES
    CXX_EXTENSIONS FALSE
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED TRUE
)
