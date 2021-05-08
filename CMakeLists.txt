cmake_minimum_required(VERSION 3.5)

project(polarGB)
set(PROJECT_LIB_NAME polargb)

# Output directories.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

###############################################################################
# COMPILER SETTINGS
###############################################################################

# Compiler
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 17)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)


###############################################################################
# Dependencies and third party
###############################################################################

# Required packages
find_package(Boost 1.71.0 REQUIRED COMPONENTS program_options)
find_package(SDL2 REQUIRED)

# Third party
add_subdirectory(third_party)


###############################################################################
# Project library
###############################################################################

add_library(${PROJECT_LIB_NAME}
    ${PROJECT_SOURCE_DIR}/src/cartridge.cpp
    ${PROJECT_SOURCE_DIR}/src/cpu.cpp
    ${PROJECT_SOURCE_DIR}/src/emulator.cpp
    ${PROJECT_SOURCE_DIR}/src/graphics_controller.cpp
    ${PROJECT_SOURCE_DIR}/src/graphics_display.cpp
    ${PROJECT_SOURCE_DIR}/src/interrupt_controller.cpp
    ${PROJECT_SOURCE_DIR}/src/log.cpp
    ${PROJECT_SOURCE_DIR}/src/mmu.cpp
    ${PROJECT_SOURCE_DIR}/src/opcodes.cpp
    ${PROJECT_SOURCE_DIR}/src/register.cpp
    ${PROJECT_SOURCE_DIR}/src/timer.cpp
)

message(status ${SDL2_INCLUDE_DIRS})
target_include_directories(${PROJECT_LIB_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include ${SDL2_INCLUDE_DIRS})

target_link_libraries(${PROJECT_LIB_NAME} PRIVATE pthread dl fmt::fmt ${SDL2_LIBRARIES})

target_compile_options(${PROJECT_LIB_NAME} PUBLIC -Wall -Wextra -Wpedantic -ggdb3)

###############################################################################
# PROJECT BUILD SPECIFICATION
###############################################################################

# Executable and link it with source files.
add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCE_DIR}/src/main.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE boost_program_options fmt::fmt pthread ${PROJECT_LIB_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include ${Boost_INCLUDE_DIRS})


if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message("Setting build type to 'RelWithDebInfo' as none was specified.")
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()

# Compile flags
add_compile_options(-pipe)
target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wextra -Wpedantic -D_FORTIFY_SOURCE=2
                                              -fstack-protector)

###############################################################################
# TESTS
###############################################################################

enable_testing()
add_subdirectory(tests)
