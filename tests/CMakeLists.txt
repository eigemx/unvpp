enable_testing()

add_executable(
  test_reader
  test_reader.cpp
)

target_link_libraries(
    test_reader
    GTest::gtest_main
    Unvpp::unvpp
)

include(GoogleTest)

gtest_discover_tests(test_reader)

add_test(NAME test_reader COMMAND test_reader)
set_tests_properties(test_reader PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)