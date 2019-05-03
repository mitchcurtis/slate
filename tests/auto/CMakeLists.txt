# tests/auto/CMakeLists.txt
add_executable(
  test-app
)

target_sources(
  test-app
  PRIVATE
  tst_app.cpp
  ${CMAKE_SOURCE_DIR}/app/application.h
  ${CMAKE_SOURCE_DIR}/app/application.cpp
  ${CMAKE_CURRENT_LIST_DIR}/../shared/testhelper.h
  ${CMAKE_CURRENT_LIST_DIR}/../shared/testhelper.cpp
  ${CMAKE_CURRENT_LIST_DIR}/../shared/testutils.h

  resources.qrc
  ${CMAKE_SOURCE_DIR}/app/qml/qml.qrc

  ${CMAKE_SOURCE_DIR}/app/fonts/fonts.qrc
  ${CMAKE_SOURCE_DIR}/app/images/images.qrc
)

target_link_libraries(
  test-app
  PRIVATE
  slate
  projectWarning
  Qt5::Core
  Qt5::Quick
  Qt5::Test
  Qt5::Widgets
)

target_include_directories(
  test-app
  PRIVATE
  ${CMAKE_SOURCE_DIR}/app
  ${CMAKE_CURRENT_LIST_DIR}/../shared
)

set_target_properties(
  test-app
  PROPERTIES
  CXX_EXTENSIONS FALSE
  CXX_STANDARD 11
  CXX_STANDARD_REQUIRED TRUE
)

target_compile_definitions(
  test-app
  PRIVATE
  QT_DEPRECATED_WARNINGS
)

add_test(
  test-app
  test-app
)
