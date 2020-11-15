cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
cmake_policy(VERSION 2.6)

project(Delete_Command)


add_library(Delete_Command "Delete_Command.c")

target_include_directories(Delete_Command PRIVATE "include" "../../../include")
