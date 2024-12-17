
cmake_minimum_required(VERSION 3.23)

set(CMAKE_BUILD_TYPE_INIT Release)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Establish project

project(nanovdb2raw LANGUAGES C CXX)

include(GNUInstallDirs)

# Add executable

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE
  $<BUILD_INTERFACE:nanovdb2raw_nanovdb>)

# Add nanovdb dependency as an interface project
add_subdirectory(nanovdb)
