cmake_minimum_required(VERSION 3.10)

project(HackAssemblerLib)

set (CMAKE_CXX_STANDARD 20)


file(GLOB SRC_FILES src/*.cpp)
file(GLOB HEADER_FILES src/*.h)


add_library(HackAssemblerLib ${HEADER_FILES} ${SRC_FILES})
target_include_directories(HackAssemblerLib PUBLIC src)
