cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
cmake_policy(VERSION 2.6)

project(Reloadmodule_Command)


add_library(Reloadmodule_Command "Reloadmodule_Command.c")

target_include_directories(Reloadmodule_Command PRIVATE "include")
