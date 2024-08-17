cmake_minimum_required(VERSION 3.10)

project(bt)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_BUILD_TYPE Debug)

add_executable(bt
    src/bencode.hpp
    src/file_parser.cpp
    src/file_parser.hpp
    src/hasher.cpp
    src/hasher.hpp
    src/test.cpp
    src/torrent_file.cpp
    src/torrent_file.hpp
    src/url.cpp
    src/url.hpp
    src/utils.cpp
    src/utils.hpp)

find_package(cryptopp CONFIG REQUIRED)
target_link_libraries(bt PRIVATE cryptopp::cryptopp)

find_package(OpenSSL CONFIG REQUIRED)
target_link_libraries(bt PRIVATE OpenSSL::SSL)