cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
cmake_policy(VERSION 2.6)

project(Setstate_Command)


add_library(Setstate_Command "Setstate_Command.c")

target_include_directories(Setstate_Command PRIVATE "include" "../../../include")
