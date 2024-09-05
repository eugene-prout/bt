cmake_minimum_required(VERSION 3.10)

project(bt)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_BUILD_TYPE Debug)

add_executable(bt
    src/commands.cpp
    src/commands.hpp
    src/file_parser.cpp
    src/file_parser.hpp
    src/hasher.cpp
    src/hasher.hpp
    src/http_client.cpp
    src/http_client.hpp
    src/main.cpp
    src/messages.cpp
    src/messages.hpp
    src/peer.cpp
    src/peer.hpp
    src/piece.cpp
    src/piece.hpp
    src/tcp_client.cpp
    src/tcp_client.hpp
    src/torrent_file.cpp
    src/torrent_file.hpp
    src/tracker.cpp
    src/tracker.hpp
    src/url.cpp
    src/url.hpp
    src/utils.cpp
    src/utils.hpp)

add_library(bt_lib INTERFACE)
target_include_directories(bt_lib INTERFACE
    lib
)
target_link_libraries(bt PRIVATE bt_lib)


# find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
# set(CLANG_TIDY_COMMAND "${CLANG_TIDY_EXE}" "-checks=-modernize-use-trailing-return-type")
# set_target_properties(bt PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")

find_package(cryptopp CONFIG REQUIRED)
target_link_libraries(bt PRIVATE cryptopp::cryptopp)

find_package(OpenSSL CONFIG REQUIRED)
target_link_libraries(bt PRIVATE OpenSSL::SSL)