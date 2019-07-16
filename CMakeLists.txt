cmake_minimum_required(VERSION 2.6)


###############################################################################
# VARIABLES USED IN THIS SCRIPT
###############################################################################

# Project name and its version
set(PROJECT_NAME polarGB)
set(TEST_BIN_NAME runTests)
project(${PROJECT_NAME})
set(${PROJECT_NAME}_VERSION_MAJOR 0)
set(${PROJECT_NAME}_VERSION_MINOR 0)

# Compiler
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_COMPILER gcc)
set(CMAKE_C_STANDARD 11)

# Directory variables
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(TEST_DIR ${PROJECT_SOURCE_DIR}/tests)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


###############################################################################
# PROJECT BUILD SPECIFICATION
###############################################################################

# Required packages
find_package(Boost REQUIRED program_options)
find_package(glfw3 REQUIRED)

# Find all the source files
file(GLOB PROJECT_SRC_FILES
    "${SRC_DIR}/*.h"
    "${SRC_DIR}/*.cpp"
)

# Add the sources to the executable.
add_executable(${PROJECT_NAME} ${PROJECT_SRC_FILES})

# Compile flags
target_compile_options(${PROJECT_NAME} PUBLIC -O2 -Wall -Wextra -Wpedantic -D_FORTIFY_SOURCE=2 -fstack-protector -fPIE)

# Set the link libraries
target_link_libraries(${PROJECT_NAME} boost_program_options glfw GL X11 pthread Xrandr Xi dl)

# Add include directories
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIRS})


###############################################################################
# TESTS
###############################################################################

# enable_testing()
find_package(GTest REQUIRED)

# Find all the source files
file(GLOB TEST_SRC_FILES
    "${TEST_DIR}/*.h"
    "${TEST_DIR}/*.cpp"
)

add_executable(${TEST_BIN_NAME} ${TEST_SRC_FILES} ${SRC_DIR}/register.cpp)

target_include_directories(${TEST_BIN_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${TEST_BIN_NAME} PUBLIC ${GTEST_INCLUDE_DIRS})
target_include_directories(${TEST_BIN_NAME} PUBLIC ${Boost_INCLUDE_DIRS})

target_link_libraries(${TEST_BIN_NAME} ${GTEST_LIBRARIES} pthread)
target_link_libraries(${TEST_BIN_NAME} boost_program_options glfw GL X11 pthread Xrandr Xi dl)
