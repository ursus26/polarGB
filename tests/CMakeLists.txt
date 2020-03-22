
# Builds tests automatically.
#   test_name:          Name of the test that is build.
#   test_source:        Source file containing the test.
#   test_target_source: Target source file that will be tested.
# function(test_builder test_name test_source test_target_source)
#
#     add_executable(${test_name} ${test_source}.cpp ${PROJECT_SOURCE_DIR}/src/${test_target_source})
#
#
#
#   add_fmt_executable(${name} ${name}.cc ${ARGN})
#   target_link_libraries(${name} test-main)
#
#   # Define if certain C++ features can be used.
#   if (FMT_PEDANTIC)
#     target_compile_options(${name} PRIVATE ${PEDANTIC_COMPILE_FLAGS})
#   endif ()
#   if (FMT_WERROR)
#     target_compile_options(${name} PRIVATE ${WERROR_FLAG})
#   endif ()
#   target_include_directories(${name} SYSTEM PUBLIC gtest gmock)
#   add_test(NAME ${name} COMMAND ${name})
# endfunction()
#





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

add_test(NAME runTests COMMAND runTests)



# test_builder(register_test)
