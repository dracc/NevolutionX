cmake_minimum_required(VERSION 3.7)

project(NevolutionX)

INCLUDE(FindPkgConfig)

PKG_SEARCH_MODULE(SDL2TTF REQUIRED SDL2_ttf)
find_package(SDL2 REQUIRED)
find_package(Threads REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS} ${THREAD_INCLUDE_DIRS} ${SDL2TTF_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} main.c navigateMenu.c outputLine.c subsystems.c)

set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 11)

target_link_libraries(${PROJECT_NAME}
  ${SDL2_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
  ${SDL2TTF_LIBRARIES})
