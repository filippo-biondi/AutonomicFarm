cmake_minimum_required(VERSION 3.16)
project(AutonomicFarm)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")

add_definitions(-DPIN_TO_CORES=1)
set(BUILD_TEST ON)

include_directories(./include
                    ./fastflow)

file(GLOB SRC_FILES_NATIVE src/native/*.c*)
file(GLOB SRC_FILES_FASTFLOW src/fastflow/*.c*)
file(GLOB SRC_FILES_COMMON src/*.c*)

add_library(${PROJECT_NAME} SHARED ${SRC_FILES_COMMON} ${SRC_FILES_NATIVE} ${SRC_FILES_FASTFLOW})


if (BUILD_TEST)
    add_subdirectory(test)
endif ()