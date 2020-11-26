cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
cmake_policy(VERSION 2.6)

project(Reloadcommand_Command)


add_library(Reloadcommand_Command "Reloadcommand_Command.c")

target_include_directories(Reloadcommand_Command PRIVATE "include")
