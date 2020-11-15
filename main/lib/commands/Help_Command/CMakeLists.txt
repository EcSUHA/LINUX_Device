cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
cmake_policy(VERSION 2.6)

project(Help_Command)


add_library(Help_Command "Help_Command.c")

target_include_directories(Help_Command PRIVATE "include" "../../../include")
