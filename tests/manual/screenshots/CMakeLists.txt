# tests/manual/screenshots/CMakeLists.txt
add_executable(screenshots)

target_sources(screenshots
    PRIVATE
        ${CMAKE_SOURCE_DIR}/app/application.h
        ${CMAKE_SOURCE_DIR}/app/application.cpp
        ${CMAKE_SOURCE_DIR}/app/config.qrc
        ${CMAKE_SOURCE_DIR}/app/fonts/fonts.qrc
        ${CMAKE_SOURCE_DIR}/app/images/images.qrc
        ${CMAKE_CURRENT_LIST_DIR}/resources/resources.qrc
        ${CMAKE_CURRENT_SOURCE_DIR}/../../shared/testhelper.cpp
        screenshots.cpp
        ${CMAKE_SOURCE_DIR}/app/qml/qml.qrc
)

target_compile_definitions(screenshots
    PRIVATE
        APP_VERSION="${PROJECT_VERSION}"
)

find_package(Qt6 COMPONENTS Core Gui Qml Quick QuickControls2 QuickTest)

target_link_libraries(screenshots
    PRIVATE
        slate
        projectWarning
        Qt::Core
        Qt::Quick
        Qt::QuickControls2
        Qt::QuickTest
        Qt::Test
        Qt::Widgets
)

target_include_directories(screenshots
    PRIVATE
        ${CMAKE_SOURCE_DIR}/app
        ${CMAKE_CURRENT_SOURCE_DIR}/../../shared
)

set_target_properties(screenshots
    PROPERTIES
    CXX_EXTENSIONS FALSE
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED TRUE
)

target_compile_definitions(screenshots
    PRIVATE
    QT_DEPRECATED_WARNINGS
)

add_test(
    screenshots
    screenshots
)
