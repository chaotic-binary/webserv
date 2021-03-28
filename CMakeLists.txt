cmake_minimum_required(VERSION 3.17)
project(webserv)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror")
include_directories(include)

add_executable(test_parser
        test/test_parser.cpp
        src/Server.cpp
        src/Parser.cpp
        src/ServConfig.cpp
        src/Location.cpp
        src/utils.cpp
        src/Cgi.cpp
        src/Request.cpp
        src/Client.cpp
        src/MimeTypes.cpp
        src/response.cpp
        src/method_utils.cpp
        src/GetMethod.cpp
        src/methods.cpp
        src/PostMethod.cpp
        src/methods.cpp
        src/HeadMethod.cpp
        src/PutMethod.cpp
        )

add_executable(test_server
        src/Server.cpp
        src/Parser.cpp
        src/ServConfig.cpp
        src/Location.cpp
        src/utils.cpp
        src/Cgi.cpp
        src/main.cpp
        src/Request.cpp
        src/Client.cpp
        src/MimeTypes.cpp
        src/response.cpp
        src/method_utils.cpp
        src/GetMethod.cpp
        src/methods.cpp
        src/PostMethod.cpp
        src/methods.cpp
        src/HeadMethod.cpp
        src/PutMethod.cpp
        src/response_generator.cpp
        src/base64.cpp)
