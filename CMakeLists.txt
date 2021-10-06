cmake_minimum_required(VERSION 3.7)

project(NevolutionX)

INCLUDE(FindPkgConfig)

PKG_SEARCH_MODULE(SDL2TTF REQUIRED SDL2_ttf)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image)
find_package(SDL2 REQUIRED)
find_package(Threads REQUIRED)

include_directories(
  ${CMAKE_SOURCE_DIR}
  ${SDL2_INCLUDE_DIRS}
  ${THREAD_INCLUDE_DIRS}
  ${SDL2TTF_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/Includes
  )

add_executable(${PROJECT_NAME}
  main.cpp
  Includes/audioMenu.cpp
  Includes/config.cpp
  Includes/font.cpp
  Includes/ftpConnection.cpp
  Includes/ftpServer.cpp
  Includes/infoLog.cpp
  Includes/langMenu.cpp
  Includes/logViewer.cpp
  Includes/logViewerMenu.cpp
  Includes/menu.cpp
  Includes/networkManager.cpp
  Includes/renderer.cpp
  Includes/settingsMenu.cpp
  Includes/sntpClient.cpp
  Includes/subAppRouter.cpp
  Includes/subsystems.cpp
  Includes/timing.cpp
  Includes/timeMenu.cpp
  Includes/videoMenu.cpp
  Includes/wipeCache.cpp
  Includes/xbeLauncher.cpp
  Includes/xbeScanner.cpp
  3rdparty/SDL_FontCache/SDL_FontCache.c
  )

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

target_link_libraries(${PROJECT_NAME}
  ${SDL2TTF_LIBRARIES}
  ${SDL2IMAGE_LIBRARIES}
  ${SDL2_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT})

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()

#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
#set (CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
