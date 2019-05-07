add_library(MD5HashCalculator SHARED "${CMAKE_CURRENT_LIST_DIR}/MD5HashCalculator.cpp"
									"${CMAKE_CURRENT_LIST_DIR}/MD5HashCalculator.h")
target_include_directories(MD5HashCalculator INTERFACE "${CMAKE_CURRENT_LIST_DIR}")

target_link_libraries(MD5HashCalculator InterfaceLib)

add_executable(md5_test_suite "${CMAKE_CURRENT_LIST_DIR}/unit_tests/md5_test.cpp")

target_compile_definitions(md5_test_suite PRIVATE BOOST_TEST_MAIN BOOST_TEST_DYN_LINK BOOST_TEST_MODULE=md5_test_suite)

target_link_libraries(md5_test_suite Boost::unit_test_framework
									 InterfaceLib
									 MD5HashCalculator)

add_test(NAME test_runner COMMAND md5_test_suite)
