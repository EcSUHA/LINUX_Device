cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
cmake_policy(VERSION 2.6)

project(Get_Command)


add_library(Get_Command "Get_Command.c")

target_include_directories(Get_Command PRIVATE "include" "../../../include")
