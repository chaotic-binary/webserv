cmake_minimum_required(VERSION 3.17)
project(webserv)

set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror")
include_directories(include)

add_executable(test_parser
        src/main.cpp
        src/ServConfig.cpp include/ServConfig.hpp
        src/Parser.cpp include/Parser.hpp
        src/Location.cpp include/Location.hpp
        src/utils.cpp include/utils.hpp
        src/FD.cpp include/FD.hpp
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
        src/tmpFunctionForResponse.cpp
        src/king_Response.cpp
        src/initMimeType.cpp)
