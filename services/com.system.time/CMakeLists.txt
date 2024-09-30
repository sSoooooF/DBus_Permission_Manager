cmake_minimum_required(VERSION 3.10)
project(DBusServices)

find_package(sdbus-c++ REQUIRED)
find_package(SQLite3 REQUIRED)

include_directories(include)

add_executable(time_service time_server.cpp)
target_link_libraries(time_service PRIVATE sdbus-c++ SQLite::SQLite3)