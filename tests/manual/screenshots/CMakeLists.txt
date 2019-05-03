# tests/manual/CMakeLists.txt
add_executable(
  screenshots
)

target_sources(
  screenshots
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

target_link_libraries(
  screenshots
  PRIVATE
  slate
  projectWarning
  Qt5::Core
  Qt5::Quick
  Qt5::Test
  Qt5::Widgets
)

target_include_directories(
  screenshots
  PRIVATE
  ${CMAKE_SOURCE_DIR}/app
  ${CMAKE_CURRENT_SOURCE_DIR}/../../shared
)

set_target_properties(
  screenshots
  PROPERTIES
  CXX_EXTENSIONS FALSE
  CXX_STANDARD 11
  CXX_STANDARD_REQUIRED TRUE
)

target_compile_definitions(
  screenshots
  PRIVATE
  QT_DEPRECATED_WARNINGS
)

add_test(
  screenshots
  screenshots
)
