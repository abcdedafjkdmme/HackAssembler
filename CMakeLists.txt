cmake_minimum_required(VERSION 3.7...3.28)

project(HackAssembler)

add_subdirectory(HackAssemblerLib)
add_subdirectory(tests)
target_link_libraries(UnitTest HackAssemblerLib)
