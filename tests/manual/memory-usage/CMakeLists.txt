# tests/manual/memory-usage/CMakeLists.txt
add_executable(memory-usage)

target_sources(memory-usage
    PRIVATE
        ${CMAKE_SOURCE_DIR}/app/application.h
        ${CMAKE_SOURCE_DIR}/app/application.cpp
        ${CMAKE_SOURCE_DIR}/app/config.qrc
        ${CMAKE_SOURCE_DIR}/app/fonts/fonts.qrc
        ${CMAKE_SOURCE_DIR}/app/images/images.qrc
        ${CMAKE_CURRENT_SOURCE_DIR}/../../shared/testhelper.cpp
        memory-usage.cpp
        ${CMAKE_SOURCE_DIR}/app/qml/qml.qrc
)

target_compile_definitions(memory-usage
    PRIVATE
        APP_VERSION="${PROJECT_VERSION}"
)

find_package(Qt6 COMPONENTS Core Gui Qml Quick QuickControls2 QuickTest)

target_link_libraries(memory-usage
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

target_include_directories(memory-usage
    PRIVATE
        ${CMAKE_SOURCE_DIR}/app
        ${CMAKE_CURRENT_SOURCE_DIR}/../../shared
)

set_target_properties(memory-usage
    PROPERTIES
    CXX_EXTENSIONS FALSE
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED TRUE
)

target_compile_definitions(memory-usage
    PRIVATE
    QT_DEPRECATED_WARNINGS
)

add_test(
    memory-usage
    memory-usage
)
