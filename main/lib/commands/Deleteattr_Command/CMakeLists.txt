cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
cmake_policy(VERSION 2.6)

project(Deleteattr_Command)


add_library(Deleteattr_Command "Deleteattr_Command.c")

target_include_directories(Deleteattr_Command PRIVATE "include" "../../../include")
