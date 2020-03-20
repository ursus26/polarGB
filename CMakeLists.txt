cmake_minimum_required(VERSION 3.5)

project(polarGB)


###############################################################################
# COMPILER SETTINGS
###############################################################################

# Compiler
# set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 11)
# set(CMAKE_C_COMPILER gcc)
set(CMAKE_C_STANDARD 11)

###############################################################################
# PROJECT BUILD SPECIFICATION
###############################################################################

# Required packages
find_package(Boost 1.46.1 REQUIRED COMPONENTS program_options)
find_package(glfw3 REQUIRED)

# Find all the source files
file(GLOB SOURCES "src/*.cpp")

# Executable and link it with source files.
add_executable(${PROJECT_NAME} ${SOURCES})

# Add include directories
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIRS})

# Set the link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE boost_program_options glfw GL X11 pthread Xrandr Xi dl)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message("Setting build type to 'RelWithDebInfo' as none was specified.")
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()

# Compile flags
target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wextra -Wpedantic -D_FORTIFY_SOURCE=2
                       -fstack-protector -fPIE)


###############################################################################
# TESTS
###############################################################################

add_subdirectory(third_party)
enable_testing()


# Find all the source files
file(GLOB TEST_SRC_FILES
    "${PROJECT_SOURCE_DIR}/tests/*.h"
    "${PROJECT_SOURCE_DIR}/tests/*.cpp"
)

add_executable(runTests ${TEST_SRC_FILES} ${PROJECT_SOURCE_DIR}/src/register.cpp)

target_include_directories(runTests PUBLIC ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
target_include_directories(runTests PUBLIC ${PROJECT_SOURCE_DIR}/include ${Boost_INCLUDE_DIRS})

target_link_libraries(runTests gtest gtest_main)
target_link_libraries(runTests boost_program_options glfw GL X11 pthread Xrandr Xi dl)
