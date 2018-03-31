cmake_minimum_required(VERSION 3.10)
project(qtx)

set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED COMPONENTS Core)

add_library(qtx STATIC)
set_property(TARGET qtx PROPERTY CXX_STANDARD 17)

target_sources(qtx PRIVATE
    "WorkerThread.cpp"
)

target_link_libraries(qtx Qt5::Core)
