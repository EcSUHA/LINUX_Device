cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
cmake_policy(VERSION 2.6)

project(Rename_Command)


add_library(Rename_Command "Rename_Command.c")

target_include_directories(Rename_Command PRIVATE "include" "../../../include")
