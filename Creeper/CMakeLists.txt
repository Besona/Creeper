cmake_minimum_required(VERSION 3.27)
project(Creeper)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O0")

add_executable(
        ${PROJECT_NAME}
        src/main.cpp
        src/diamond.cpp)

include_directories(
        include
)
