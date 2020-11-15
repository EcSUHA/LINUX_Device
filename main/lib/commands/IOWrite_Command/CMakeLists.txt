cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
cmake_policy(VERSION 2.6)

project(IOWrite_Command)


add_library(IOWrite_Command "IOWrite_Command.c")

target_include_directories(IOWrite_Command PRIVATE "include" "../../../include")
