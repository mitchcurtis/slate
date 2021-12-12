# app/CMakeLists.txt
if(WIN32)
    add_executable(
        app
        WIN32
    )
else()
    add_executable(
        app
    )
endif()

target_sources(
    app
    PRIVATE
    application.h
    application.cpp
    main.cpp

    config.qrc
    fonts/fonts.qrc
    images/images.qrc

    qml/qml.qrc
)

find_package(Qt6 COMPONENTS Core)
find_package(Qt6 COMPONENTS Gui)
find_package(Qt6 COMPONENTS Qml)
find_package(Qt6 COMPONENTS Quick)
find_package(Qt6 COMPONENTS Widgets)

target_link_libraries(
    app
    PRIVATE
    slate
    projectWarning
    Qt::Core
    Qt::Quick
    Qt::Widgets
)

set_target_properties(
    app
    PROPERTIES
    CXX_EXTENSIONS FALSE
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED TRUE
)
