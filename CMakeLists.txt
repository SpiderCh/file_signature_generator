cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 17)
project("signature_generator")

set(Boost_INCLUDE_DIR "${SDK_PATH}/boost/1.70/include/")
set(Boost_LIBRARY_DIR "${SDK_PATH}/boost/1.70/lib/")

find_package(Boost 1.70 COMPONENTS program_options unit_test_framework REQUIRED)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

include("${SRC_DIR}/interfaces/Interface.cmake")
include("${SRC_DIR}/lib/FileHashSaver/FileHashSaver.cmake")
include("${SRC_DIR}/lib/FileDataProvider/FileDataProvider.cmake")
include("${SRC_DIR}/lib/MD5HashCalculator/MD5HashCalculator.cmake")
include("${SRC_DIR}/lib/CRCHashCalculator/CRCHashCalculator.cmake")

add_executable(${PROJECT_NAME}  ${SRC_DIR}/app/main.cpp
								${SRC_DIR}/app/SignatureCalculator.h
								${SRC_DIR}/app/SignatureCalculator.cpp)

target_link_libraries(${PROJECT_NAME} Boost::program_options
									  InterfaceLib
									  FileHashSaver
									  FileDataProvider
									  MD5HashCalculator
									  CRCHashCalculator)
