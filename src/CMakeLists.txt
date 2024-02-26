cmake_minimum_required(VERSION 3.10)

project(HackAssemblerLib)

set (CMAKE_CXX_STANDARD 20)


file(GLOB SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)


add_library(HackAssemblerLib ${HEADER_FILES} ${SRC_FILES})
target_include_directories(HackAssemblerLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
