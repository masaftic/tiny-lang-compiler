cmake_minimum_required(VERSION 3.5)

project(QtHelloWorld)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON) # Enable automatic MOC handling for Qt
set(CMAKE_AUTOUIC ON) # Enable automatic UIC handling for Qt
set(CMAKE_AUTORCC ON) # Enable automatic RCC handling for Qt

find_package(Qt5 COMPONENTS Widgets REQUIRED)

# Add all source files needed for the executable
add_executable(QtHelloWorld
    qt.cpp
    lexer.cpp  # Add lexer implementation
    token.cpp  # Add token implementation
    parser.cpp
)

target_link_libraries(QtHelloWorld Qt5::Widgets)
