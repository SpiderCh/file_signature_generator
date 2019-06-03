add_library(CRCHashCalculator SHARED "${CMAKE_CURRENT_LIST_DIR}/CRCHashCalculator.cpp"
									"${CMAKE_CURRENT_LIST_DIR}/CRCHashCalculator.h")
target_include_directories(CRCHashCalculator INTERFACE "${CMAKE_CURRENT_LIST_DIR}")

target_link_libraries(CRCHashCalculator InterfaceLib)

add_executable(crc_test_suite "${CMAKE_CURRENT_LIST_DIR}/unit_tests/crc_test.cpp")

target_compile_definitions(crc_test_suite PRIVATE BOOST_TEST_MAIN BOOST_TEST_DYN_LINK BOOST_TEST_MODULE=crc_test_suite)

target_link_libraries(crc_test_suite Boost::unit_test_framework
									 InterfaceLib
									 CRCHashCalculator)

add_test(NAME test_runner COMMAND crc_test_suite)
