cmake_minimum_required(VERSION 3.10)
project(com.system.time)

set(CMAKE_CXX_STANDARD 17)

find_package(sdbus-c++ REQUIRED)

add_executable(time_service src/main.cpp src/TimeService.cpp)

target_link_libraries(time_service sdbus-c++::sdbus-c++)
