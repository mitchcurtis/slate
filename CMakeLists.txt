# CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project(slate VERSION 0.10.0 LANGUAGES CXX C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

option(ENABLE_TESTING "" ON)

find_package(Qt6 COMPONENTS Core Gui Qml Quick Widgets Test LinguistTools)

qt_policy(SET QTP0001 NEW)

add_library(projectWarning INTERFACE)

target_compile_options(projectWarning INTERFACE
    $<$<CXX_COMPILER_ID:GNU>:$<BUILD_INTERFACE:-Wall;-Wextra;-Wpedantic>>
    $<$<CXX_COMPILER_ID:Clang>:$<BUILD_INTERFACE:-Weverything;-Wpedantic>>
    $<$<CXX_COMPILER_ID:MSVC>:/W0>
)

set(ts_files
    translations/slate_ar_EG.ts
    translations/slate_nb_NO.ts
    translations/slate_zh_CN.ts
)
set_source_files_properties(${ts_files}
    PROPERTIES OUTPUT_LOCATION
        "${CMAKE_CURRENT_BINARY_DIR}/app/translations"
)
qt_add_translations(slate
    TS_FILES
        ${ts_files}
    SOURCES
        app/qml/qml.qrc
)

add_subdirectory(lib)
add_subdirectory(app)

if(ENABLE_TESTING)
    enable_testing()

    add_subdirectory(tests)
endif()

set_property(DIRECTORY tests PROPERTY QT_EXCLUDE_FROM_TRANSLATION ON)

