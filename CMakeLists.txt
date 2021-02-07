cmake_minimum_required(VERSION 3.17)
project(webserv)

set(CMAKE_CXX_STANDARD 98)

add_executable(webserv src/main.cpp src/ServConfig.cpp include/ServConfig.hpp src/Parser.cpp include/Parser.hpp)