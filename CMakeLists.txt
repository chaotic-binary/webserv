cmake_minimum_required(VERSION 3.17)
project(webserv)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror")
include_directories(include)

add_executable(test_parser
        src/main.cpp
        src/ServConfig.cpp
        src/Parser.cpp
        src/Location.cpp
        src/utils.cpp
        src/FD.cpp
        )

add_executable(test_server
        src/Server.cpp
        src/Parser.cpp
        src/ServConfig.cpp
        src/Location.cpp
        src/utils.cpp
        src/FD.cpp
        src/Cgi.cpp
        test/testServer.cpp
        src/Request.cpp
        src/Client.cpp
        src/initMimeType.cpp
        src/response.cpp)
