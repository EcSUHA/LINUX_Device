cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
cmake_policy(VERSION 2.6)

project(Save_Command)


add_library(Save_Command "Save_Command.c")

target_include_directories(Save_Command PRIVATE "include" "../../../include")
