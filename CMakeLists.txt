cmake_minimum_required(VERSION 3.7)

project(NevolutionX)

INCLUDE(FindPkgConfig)

PKG_SEARCH_MODULE(SDL2TTF REQUIRED SDL2_ttf)
find_package(SDL2 REQUIRED)
find_package(Threads REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS} ${THREAD_INCLUDE_DIRS} ${SDL2TTF_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/Includes)

add_executable(${PROJECT_NAME}
  main.cpp
  Includes/outputLine.cpp
  Includes/subsystems.cpp
  Includes/menuItem.cpp
  Includes/xbeMenuItem.cpp
  Includes/renderer.cpp
  Includes/findXBE.cpp
  Includes/font.cpp
  Includes/xpadinput.cpp
  )

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)

target_link_libraries(${PROJECT_NAME}
  ${SDL2_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
  ${SDL2TTF_LIBRARIES})
