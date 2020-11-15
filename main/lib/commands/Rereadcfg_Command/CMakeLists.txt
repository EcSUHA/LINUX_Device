cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
cmake_policy(VERSION 2.6)

project(Rereadcfg_Command)


add_library(Rereadcfg_Command "Rereadcfg_Command.c")

target_include_directories(Rereadcfg_Command PRIVATE "include" "../../../include")
