cmake_minimum_required(VERSION 3.16)
project(AutonomicFarmTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")

include_directories(../include)

add_executable(test_native test_native.cpp)
add_executable(test_fastflow test_fastflow.cpp)

target_link_libraries(test_native AutonomicFarm)
target_link_libraries(test_fastflow AutonomicFarm)