cmake_minimum_required(VERSION 3.16)
project(AutonomicFarm)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(native_af)

include_directories(native_af/include)

add_executable(test1 test/test1.cpp)
target_link_libraries(test1 PRIVATE NativeAutonomicFarm)

add_executable(test3 test/test_manager.cpp)
target_link_libraries(test3 PRIVATE NativeAutonomicFarm)
