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

target_link_libraries(
  app
  PRIVATE
  slate
  projectWarning
  Qt5::Core
  Qt5::Quick
  Qt5::Widgets
)

set_target_properties(
  app
  PROPERTIES
  CXX_EXTENSIONS FALSE
  CXX_STANDARD 11
  CXX_STANDARD_REQUIRED TRUE
)
