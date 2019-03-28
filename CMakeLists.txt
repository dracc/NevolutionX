cmake_minimum_required(VERSION 3.7)

project(NevolutionX)

set(CC gcc)

find_package(SDL2 REQUIRED)
find_package(Threads REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} main.c)
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} ${THREAD_LIBRARIES})
