cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
cmake_policy(VERSION 2.6)

project(Define_Command)


add_library(Define_Command "Define_Command.c")

target_include_directories(Define_Command PRIVATE "include" "../../../include")
