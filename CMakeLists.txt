cmake_minimum_required(VERSION 3.10)

project(bt)

set(CMAKE_CXX_STANDARD 23)

find_package(cryptopp CONFIG REQUIRED)


add_executable(bt src/test.cpp)

target_link_libraries(bt PRIVATE cryptopp)