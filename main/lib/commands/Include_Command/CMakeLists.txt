cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
cmake_policy(VERSION 2.6)

project(Include_Command)


add_library(Include_Command "Include_Command.c")

target_include_directories(Include_Command PRIVATE "include" "../../../include")
