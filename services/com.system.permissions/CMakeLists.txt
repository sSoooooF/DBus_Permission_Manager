cmake_minimum_required(VERSION 3.10)
project(com.system.permissions)

set(CMAKE_CXX_STANDARD 17)

find_package(sdbus-c++ REQUIRED)

find_package(SQLite3 REQUIRED)

add_executable(permission_service src/main.cpp src/PermissionService.cpp)

target_link_libraries(permission_service sdbus-c++::sdbus-c++ SQLite::SQLite3)
