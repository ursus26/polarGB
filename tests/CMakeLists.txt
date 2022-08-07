set(TEST_BINARY tests)

# Find all the source files
file(GLOB TEST_SRC_FILES
    "${PROJECT_SOURCE_DIR}/tests/*.h"
    "${PROJECT_SOURCE_DIR}/tests/*.cpp"
)

add_executable(${TEST_BINARY} ${TEST_SRC_FILES})

target_include_directories(${TEST_BINARY} PRIVATE ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(${TEST_BINARY} gtest gtest_main ${PROJECT_LIB_NAME} GL X11 pthread dl fmt::fmt)

add_test(NAME ${TEST_BINARY} COMMAND ${TEST_BINARY})
