enable_testing()

add_executable(
  test_reader
  test_reader_basics.cpp
  test_reader_elements.cpp
  test_reader_groups.cpp
)


target_link_libraries(
  test_reader
  GTest::gtest_main
  Unvpp::unvpp
)

include(GoogleTest)

gtest_discover_tests(test_reader)