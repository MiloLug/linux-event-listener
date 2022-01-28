cmake_minimum_required(VERSION 3.1)

project(event-listener C)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -std=c17")


cmake_policy(SET CMP0015 NEW)

add_subdirectory(src event-listener)

