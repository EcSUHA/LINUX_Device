cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
cmake_policy(VERSION 2.6)

project(List_Command)


add_library(List_Command "List_Command.c")

target_include_directories(List_Command PRIVATE "include" "../../../include")
