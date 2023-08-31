cmake_minimum_required(VERSION 3.16)
project(AutonomicFarmTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")

include_directories(../include)

add_executable(test_native native/test_native.cpp)
add_executable(test_fastflow fastflow/test_fastflow.cpp)
add_executable(test_native_latency native/test_native_latency.cpp)
add_executable(test_fastflow_latency fastflow/test_fastflow_latency.cpp)
add_executable(test_queue native/test_queue_recovery.cpp)

target_link_libraries(test_native AutonomicFarm)
target_link_libraries(test_fastflow AutonomicFarm)
target_link_libraries(test_native_latency AutonomicFarm)
target_link_libraries(test_fastflow_latency AutonomicFarm)
target_link_libraries(test_queue AutonomicFarm)