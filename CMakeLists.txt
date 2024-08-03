cmake_minimum_required(VERSION 3.10)

project(bt)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_BUILD_TYPE Debug)

add_executable(bt src/test.cpp)

find_package(cryptopp CONFIG REQUIRED)
target_link_libraries(bt PRIVATE cryptopp::cryptopp)

find_package(OpenSSL CONFIG REQUIRED)
target_link_libraries(bt PRIVATE OpenSSL::SSL)