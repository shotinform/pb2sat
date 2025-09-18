cmake_minimum_required(VERSION 3.10)
project(sat_encoder LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(sat_encoder
    src/main.cpp
    src/core.cpp
    src/normalize.cpp
    src/seqcounter.cpp
    src/adder_network.cpp
)

target_include_directories(sat_encoder PRIVATE ${CMAKE_SOURCE_DIR}/src)