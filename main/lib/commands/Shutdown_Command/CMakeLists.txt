cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
cmake_policy(VERSION 2.6)

project(Shutdown_Command)


add_library(Shutdown_Command "Shutdown_Command.c")

target_include_directories(Shutdown_Command PRIVATE "include" "../../../include")
