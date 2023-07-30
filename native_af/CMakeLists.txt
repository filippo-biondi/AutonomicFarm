cmake_minimum_required(VERSION 3.16)
project(NativeAutonomicFarm)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")

add_definitions(-DPIN_TO_CORES=1)

include_directories(./include)

file(GLOB SRC_FILES src/*.c*)

add_library(${PROJECT_NAME} SHARED ${SRC_FILES})

