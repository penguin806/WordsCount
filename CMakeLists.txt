cmake_minimum_required(VERSION 3.14)
project(WordsCount C)

set(CMAKE_C_STANDARD 99)

add_executable(WordsCount test.c main.c)