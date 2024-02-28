cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
project(UnitTest)


include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)
enable_testing()

add_executable(UnitTest test.cpp)
target_link_libraries(UnitTest GTest::gtest_main)

gtest_add_tests(TARGET      UnitTest
                TEST_SUFFIX .noArgs
                TEST_LIST   noArgsTests
)
