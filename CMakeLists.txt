cmake_minimum_required(VERSION 2.8.9)
project(c_state_machine)

set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wall -Werror")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Werror")

add_executable(c_state_machine main.c)
