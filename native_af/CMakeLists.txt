cmake_minimum_required(VERSION 3.25)
project(NativeAutonomicFarm)

set(CMAKE_CXX_STANDARD 17)

include_directories(./include)

file(GLOB SRC_FILES src/*.c*)

add_library(${PROJECT_NAME} SHARED ${SRC_FILES})