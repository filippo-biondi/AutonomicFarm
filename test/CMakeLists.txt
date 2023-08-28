cmake_minimum_required(VERSION 3.16)
project(AutonomicFarmTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")

include_directories(../include)

add_executable(test1 test1.cpp)
add_executable(test2 test_native.cpp)
add_executable(test3 test_fastflow.cpp)

target_link_libraries(test1 AutonomicFarm)
target_link_libraries(test2 AutonomicFarm)
target_link_libraries(test3 AutonomicFarm)