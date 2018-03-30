cmake_minimum_required(VERSION 2.6)
project(myriad)

add_executable(myriad main.cpp)

install(TARGETS myriad RUNTIME DESTINATION bin)
