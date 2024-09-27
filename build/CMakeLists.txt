cmake_minimum_required(VERSION 3.10)
project(PermissionManager)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDBUS REQUIRED sdbus-c++)

add_executable(myapp main.cpp)

target_include_directories(myapp PRIVATE ${SDBUS_INCLUDE_DIRS})
target_link_libraries(myapp PRIVATE ${SDBUS_LIBRARIES})