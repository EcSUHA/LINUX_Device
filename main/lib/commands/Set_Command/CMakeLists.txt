cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
cmake_policy(VERSION 2.6)

project(Set_Command)


add_library(Set_Command "Set_Command.c")

target_include_directories(Set_Command PRIVATE "include" "../../../include")
