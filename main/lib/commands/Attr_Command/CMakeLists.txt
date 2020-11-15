cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
cmake_policy(VERSION 2.6)

project(Attr_Command)


add_library(Attr_Command "Attr_Command.c")

target_include_directories(Attr_Command PRIVATE "include" "../../../include")
