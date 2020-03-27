set(TEST_BINARY tests)

# Find all the source files
file(GLOB TEST_SRC_FILES
    "${PROJECT_SOURCE_DIR}/tests/*.h"
    "${PROJECT_SOURCE_DIR}/tests/*.cpp"
)

add_executable(${TEST_BINARY} ${TEST_SRC_FILES}
                              ${PROJECT_SOURCE_DIR}/src/cartridge.cpp
                              ${PROJECT_SOURCE_DIR}/src/mmu.cpp
                              ${PROJECT_SOURCE_DIR}/src/register.cpp)

target_include_directories(${TEST_BINARY} PUBLIC ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
target_include_directories(${TEST_BINARY} PUBLIC ${PROJECT_SOURCE_DIR}/include ${Boost_INCLUDE_DIRS})

target_link_libraries(${TEST_BINARY} gtest gtest_main)
target_link_libraries(${TEST_BINARY} boost_program_options glfw GL X11 pthread Xrandr Xi dl fmt::fmt)

add_test(NAME ${TEST_BINARY} COMMAND ${TEST_BINARY})
