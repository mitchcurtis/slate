# tests/auto/CMakeLists.txt
add_executable(test-app)

find_package(Qt6 COMPONENTS Core Gui Qml Quick QuickControls2 QuickTest)

target_sources(test-app
    PRIVATE
        ${CMAKE_SOURCE_DIR}/app/application.h
        ${CMAKE_SOURCE_DIR}/app/application.cpp
        ${CMAKE_SOURCE_DIR}/app/fonts/fonts.qrc
        ${CMAKE_SOURCE_DIR}/app/images/images.qrc
        ${CMAKE_SOURCE_DIR}/lib/3rdparty/bitmap/bmp.h
        ${CMAKE_SOURCE_DIR}/lib/3rdparty/bitmap/bmp.c
        ${CMAKE_SOURCE_DIR}/lib/3rdparty/bitmap/misc/gif.h
        ${CMAKE_SOURCE_DIR}/lib/3rdparty/bitmap/misc/gif.c
        resources.qrc
        tst_app.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../shared/testhelper.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../shared/testhelper.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../shared/testutils.h
        ${CMAKE_SOURCE_DIR}/app/qml/qml.qrc
)

target_compile_definitions(test-app
    PRIVATE
        APP_VERSION="${PROJECT_VERSION}"
)

# Avoid each file being passed as a separate macro
# to the compiler, rather than one macro that contains a list of files.
set(ts_files_cpp ${ts_files})
string(REPLACE ";" " " ts_files_cpp "${ts_files_cpp}")
target_compile_definitions(test-app
    PRIVATE
        TS_FILES="${ts_files_cpp}"
)

target_include_directories(test-app
    PRIVATE
        ${CMAKE_SOURCE_DIR}/app
        ${CMAKE_CURRENT_SOURCE_DIR}/../shared
        ${CMAKE_SOURCE_DIR}/lib/3rdparty
)

target_link_libraries(test-app
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

set_target_properties(test-app
    PROPERTIES
    CXX_EXTENSIONS FALSE
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED TRUE
    AUTOMOC TRUE
)

target_compile_definitions(test-app
    PRIVATE
    QT_DEPRECATED_WARNINGS
)

add_test(
    test-app
    test-app
)
