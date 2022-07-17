# app/CMakeLists.txt

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick Widgets)

qt_add_executable(app WIN32 MACOSX_BUNDLE
    application.h
    application.cpp
    main.cpp

    config.qrc
    fonts/fonts.qrc
    images/images.qrc

    qml/qml.qrc
)

target_link_libraries(app
    PRIVATE
        slate
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
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED TRUE
)
