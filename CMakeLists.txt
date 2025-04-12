cmake_minimum_required(VERSION 3.5)

project(QtHelloWorld)

set(CMAKE_CXX_STANDARD 11)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(QtHelloWorld qt.cpp)

target_link_libraries(QtHelloWorld Qt5::Widgets)
