cmake_minimum_required(VERSION 2.6)

# Project info
project(polarGB)
set(polarGB_VERSION_MAJOR 0)
set(polarGB_VERSION_MINOR 0)
set(CMAKE_CXX_STANDARD 11)

# Required packages
find_package(Boost REQUIRED program_options)
find_package(glfw3 REQUIRED)

# Add include directories
include_directories(${Boost_INCLUDE_DIRS})
include_directories("./include")

# Add the sources to the executable.
add_executable(polarGB ./src/cartridge.cpp
                       ./src/cpu.cpp
                       ./src/emulator.cpp
                       ./src/glad.c
                       ./src/interrupt_controller.cpp
                       ./src/log.cpp
                       ./src/main.cpp
                       ./src/mmu.cpp
                       ./src/opcodes.cpp
                       ./src/register.cpp
                       ./src/video.cpp)

# Set the link libraries
target_link_libraries(polarGB boost_program_options glfw GL X11 pthread Xrandr Xi dl)
