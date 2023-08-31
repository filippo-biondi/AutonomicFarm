cmake_minimum_required(VERSION 3.16)
project(AutonomicFarm)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

#add_compile_options(-DFF_BOUNDED_BUFFER)
#add_compile_options(-DDEFAULT_BUFFER_CAPACITY=1)

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